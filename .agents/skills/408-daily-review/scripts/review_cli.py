#!/usr/bin/env python3
"""Deterministic CLI engine for the 408-daily-review skill.

Zero third-party dependencies (Python >= 3.9).
Subcommands:
  status    Show dynamic latest-N-years seal, due/weak/learning/skipped topics, and pending mistakes
  search    Search keywords strictly within non-sealed 408 exam PDFs
  read      Read line ranges strictly from non-sealed 408 exam PDFs
  record    Deterministically update spaced-repetition state & used_real_questions in tracker.json
  progress  Update topic statuses (learning / learned / weak / skipped / not_started)
  c-lab     Initialize or verify pure-C11 coding labs (static C++ check + Red/Green compile & test gate)
  validate  Validate tracker.json schema, mistakes.md structure, and skill layout integrity
"""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import re
import shutil
import subprocess
import sys
from collections.abc import Callable
from pathlib import Path
from typing import Any, NoReturn, Optional

TrackerDict = dict[str, Any]
TopicDict = dict[str, Any]
TopicRow = tuple[str, str, str, str, int, Optional[str], str]

SKILL_DIR: Path = Path(__file__).resolve().parents[1]
REPO_ROOT: Path = SKILL_DIR.parents[2]
EXAM_DIR: Path = REPO_ROOT / "真题" / "408"
REVIEW_DIR: Path = REPO_ROOT / "408" / "review"
TRACKER_PATH: Path = REVIEW_DIR / "tracker.json"
MISTAKES_PATH: Path = REVIEW_DIR / "mistakes.md"
CODE_ROOT: Path = REVIEW_DIR / "code"
TPL_DIR: Path = SKILL_DIR / "resources" / "c_lab_templates"

VALID_STATUSES: set[str] = {"not_started", "skipped", "learning", "learned", "weak"}
CPP_FORBIDDEN_PATTERNS: list[tuple[str, str]] = [
    (
        r"#\s*include\s*<\s*(iostream|vector|string|algorithm|map|set|unordered_map|unordered_set|queue|stack|deque|utility)\s*>|#\s*include\s*<\s*bits/",
        "C++ STL header",
    ),
    (r"\busing\s+namespace\s+std\b", "using namespace std"),
    (r"\bstd::", "std:: namespace"),
    (r"\b(new|delete)\b", "C++ new/delete keyword (use malloc/calloc/free in C)"),
    (r"\b(class|template|nullptr|constexpr)\b", "C++ keyword"),
]


def die(msg: str, code: int = 1) -> NoReturn:
    print(f"ERROR: {msg}", file=sys.stderr)
    sys.exit(code)


def load_tracker() -> TrackerDict:
    if not TRACKER_PATH.exists():
        die(f"Tracker not found: {TRACKER_PATH}")
    try:
        data: Any = json.loads(TRACKER_PATH.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        die(f"Invalid JSON in {TRACKER_PATH}: {exc}")
    if not isinstance(data, dict):
        die(f"Root of {TRACKER_PATH} must be a JSON object")
    return data


def save_tracker(tracker: TrackerDict) -> None:
    TRACKER_PATH.parent.mkdir(parents=True, exist_ok=True)
    tmp = TRACKER_PATH.with_suffix(".json.tmp")
    tmp.write_text(json.dumps(tracker, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    tmp.replace(TRACKER_PATH)


def get_exam_pools(seal_count: int) -> tuple[list[int], list[int]]:
    if not EXAM_DIR.exists():
        return [], []
    years: list[int] = sorted(
        int(m.group(1))
        for p in EXAM_DIR.glob("*.pdf")
        if (m := re.fullmatch(r"(\d{4})", p.stem)) is not None
    )
    if seal_count <= 0:
        return years, []
    if len(years) <= seal_count:
        return [], years
    return years[:-seal_count], years[-seal_count:]


def pdf_lines(year: int) -> list[str]:
    if shutil.which("pdftotext") is None:
        die("pdftotext binary not found in PATH")
    p = EXAM_DIR / f"{year}.pdf"
    if not p.exists():
        die(f"Exam PDF not found: {p}")
    out = subprocess.check_output(["pdftotext", str(p), "-"], stderr=subprocess.DEVNULL)
    return out.decode("utf-8", errors="ignore").splitlines()


def parse_pending_mistakes() -> list[tuple[str, str]]:
    if not MISTAKES_PATH.exists():
        return []
    text = MISTAKES_PATH.read_text(encoding="utf-8")
    active_section = text.split("## 已攻克")[0]
    results: list[tuple[str, str]] = []
    for m in re.finditer(r"^###\s+(M\d+)\s*·\s*(.+)$", active_section, flags=re.MULTILINE):
        results.append((m.group(1), m.group(2).strip()))
    return results


def find_topic(tracker: TrackerDict, topic_id: str) -> Optional[tuple[str, TopicDict]]:
    subjects = tracker.get("subjects", {})
    if not isinstance(subjects, dict):
        return None
    for subj_code, subj in subjects.items():
        if not isinstance(subj, dict):
            continue
        topics = subj.get("topics", [])
        if not isinstance(topics, list):
            continue
        for topic in topics:
            if isinstance(topic, dict) and topic.get("id") == topic_id:
                return str(subj_code), topic
    return None


def sha256_file(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


# ---------------------------------------------------------------------------
# Subcommands
# ---------------------------------------------------------------------------

def cmd_status(args: argparse.Namespace) -> None:
    tracker = load_tracker()
    today_str = str(args.today)
    cfg: dict[str, Any] = tracker.get("config", {})
    seal_n = int(cfg.get("seal_latest_years", 5))
    quota: dict[str, Any] = cfg.get("daily_quota", {"choice": 3, "free_response": 1})
    allowed, sealed = get_exam_pools(seal_n)

    print(f"=== 408 Daily Review Status ({today_str}) ===")
    print(f"Daily quota      : {quota.get('choice', 3)} choice + {quota.get('free_response', 1)} free-response")
    print(f"Dynamic seal     : latest {seal_n} years -> SEALED {sealed}")
    print(f"Allowed pool     : {len(allowed)} years -> {allowed}")
    used: list[Any] = tracker.get("used_real_questions", [])
    print(f"Used real exams  : {len(used)} -> {used}")

    mistakes = parse_pending_mistakes()
    print(f"Pending mistakes : {len(mistakes)}")
    for mid, title in mistakes:
        print(f"  - {mid}: {title}")

    due: list[TopicRow] = []
    weak: list[TopicRow] = []
    learning: list[TopicRow] = []
    skipped: list[TopicRow] = []

    subjects: dict[str, Any] = tracker.get("subjects", {})
    for subj_code, subj in subjects.items():
        for t in subj.get("topics", []):
            st = str(t.get("status", "not_started"))
            nr_raw = t.get("next_review")
            nr = str(nr_raw) if isinstance(nr_raw, str) else None
            row: TopicRow = (
                str(subj_code),
                str(t["id"]),
                str(t["name"]),
                st,
                int(t.get("mastery", 0)),
                nr,
                str(t.get("notes", "")),
            )
            if st == "skipped":
                skipped.append(row)
            elif st == "learning":
                learning.append(row)
            elif st == "weak":
                weak.append(row)
                if nr is not None and nr <= today_str:
                    due.append(row)
            elif st == "learned" and nr is not None and nr <= today_str:
                due.append(row)

    def dump_group(header: str, rows: list[TopicRow]) -> None:
        print(f"\n--- {header} ({len(rows)}) ---")
        if not rows:
            print("  (none)")
            return
        for sc, tid, name, st, mastery, nr, notes in rows:
            suffix = f" | {notes}" if notes else ""
            print(f"  [{sc}] {tid} ({name}) | status={st} mastery={mastery} next={nr}{suffix}")

    dump_group("DUE TODAY (next_review <= today)", due)
    dump_group("WEAK / GENTLE REVIEW", weak)
    dump_group("RECENT LEARNING", learning)
    dump_group("SKIPPED (LOCKED - DO NOT QUIZ)", skipped)

    lab_dir = CODE_ROOT / today_str
    if lab_dir.exists():
        print(f"\n--- ACTIVE C LAB ({today_str}) ---")
        print(f"  Path: {lab_dir}")


def cmd_search(args: argparse.Namespace) -> None:
    tracker = load_tracker()
    seal_n = int(tracker.get("config", {}).get("seal_latest_years", 5))
    allowed, sealed = get_exam_pools(seal_n)
    keywords: list[str] = [str(k) for k in args.keywords]
    limit = int(args.limit)
    print(f"[Seal enforced] Allowed={allowed} | Blocked={sealed}")
    for y in allowed:
        lines = pdf_lines(y)
        hits: list[tuple[int, str]] = [
            (idx, line.strip())
            for idx, line in enumerate(lines)
            if any(kw.lower() in line.lower() for kw in keywords)
        ]
        if hits:
            print(f"\n=== {y} ({len(hits)} hits) ===")
            for idx, txt in hits[:limit]:
                print(f"  L{idx}: {txt}")


def cmd_read(args: argparse.Namespace) -> None:
    tracker = load_tracker()
    seal_n = int(tracker.get("config", {}).get("seal_latest_years", 5))
    allowed, sealed = get_exam_pools(seal_n)
    year = int(args.year)
    if year in sealed:
        die(
            f"BLOCKED: Year {year} belongs to dynamically sealed latest {seal_n} years {sealed}. "
            "Reading is strictly forbidden to protect mock exams.",
            code=2,
        )
    if year not in allowed:
        die(f"Year {year} not in allowed pool {allowed}")
    lines = pdf_lines(year)
    s = max(0, int(args.start))
    e = min(len(lines), int(args.end))
    print(f"=== {year} L{s}-{e} (allowed pool) ===")
    for i in range(s, e):
        print(f"{i:4d}: {lines[i]}")


def cmd_record(args: argparse.Namespace) -> None:
    tracker = load_tracker()
    date_str = str(args.date)
    try:
        review_date = dt.date.fromisoformat(date_str)
    except ValueError:
        die(f"Invalid ISO date: {date_str}")

    intervals: list[int] = [
        int(x) for x in tracker.get("config", {}).get("spaced_intervals_days", [1, 3, 7, 15, 30])
    ]
    max_idx = len(intervals) - 1

    updates_summary: list[str] = []
    topic_specs: list[str] = [str(s) for s in args.topic]
    for spec in topic_specs:
        if ":" not in spec:
            die(f"Invalid --topic spec '{spec}', expected <topic_id>:<pass|weak|fail>")
        tid, outcome = spec.split(":", 1)
        if outcome not in {"pass", "weak", "fail"}:
            die(f"Invalid outcome '{outcome}' for {tid}; use pass, weak, or fail")
        found = find_topic(tracker, tid)
        if found is None:
            die(f"Unknown topic_id: {tid}")
        _, topic = found
        old_mastery = int(topic.get("mastery", 2) or 2)
        old_idx = int(topic.get("interval_idx", 0) or 0)

        if outcome == "pass":
            new_mastery = min(5, max(2, old_mastery) + 1)
            new_idx = min(max_idx, old_idx + 1)
            cur_status = str(topic.get("status", "learned"))
            new_status = "learned" if new_mastery >= 3 else cur_status
            if new_status in {"not_started", "skipped", "learning"}:
                new_status = "learned"
        elif outcome == "weak":
            new_mastery = max(2, old_mastery)
            new_idx = 0
            new_status = "weak"
        else:  # fail
            new_mastery = max(1, old_mastery - 1)
            new_idx = 0
            new_status = "weak"

        next_dt = review_date + dt.timedelta(days=intervals[new_idx])
        topic["mastery"] = new_mastery
        topic["interval_idx"] = new_idx
        topic["status"] = new_status
        topic["last_review"] = review_date.isoformat()
        topic["next_review"] = next_dt.isoformat()
        updates_summary.append(
            f"  {tid}: outcome={outcome} -> status={new_status}, mastery={new_mastery}, next_review={topic['next_review']}"
        )

    note_specs: list[str] = [str(n) for n in (args.note or [])]
    for nspec in note_specs:
        if ":" not in nspec:
            die(f"Invalid --note '{nspec}', expected <topic_id>:<text>")
        tid, text = nspec.split(":", 1)
        found = find_topic(tracker, tid)
        if found is None:
            die(f"Unknown topic_id in --note: {tid}")
        found[1]["notes"] = text.strip()

    used: list[str] = [str(q) for q in tracker.get("used_real_questions", [])]
    used_args: list[str] = [str(q) for q in (args.used or [])]
    for qid in used_args:
        if re.fullmatch(r"\d{4}-Q\d+", qid) is None:
            die(f"Invalid question ID '{qid}', expected format YYYY-QN (e.g. 2018-Q3)")
        if qid not in used:
            used.append(qid)
    tracker["used_real_questions"] = sorted(used)

    save_tracker(tracker)
    print(f"Updated tracker.json for {review_date.isoformat()}:")
    for line in updates_summary:
        print(line)
    if used_args:
        print(f"  used_real_questions: {tracker['used_real_questions']}")


def cmd_progress(args: argparse.Namespace) -> None:
    tracker = load_tracker()
    today_str = str(args.today)
    changed: list[str] = []
    status_specs: list[str] = [str(s) for s in args.set_status]
    for item in status_specs:
        if "=" not in item:
            die(f"Invalid --set '{item}', expected <topic_id>=<status>")
        tid, st = item.split("=", 1)
        if st not in VALID_STATUSES:
            die(f"Invalid status '{st}', must be one of {sorted(VALID_STATUSES)}")
        found = find_topic(tracker, tid)
        if found is None:
            die(f"Unknown topic_id: {tid}")
        _, topic = found
        topic["status"] = st
        if st in {"learning", "learned", "weak"} and not topic.get("next_review"):
            topic["next_review"] = today_str
            if int(topic.get("mastery", 0) or 0) == 0:
                topic["mastery"] = 2
        elif st in {"skipped", "not_started"}:
            topic["next_review"] = None
        changed.append(f"  {tid} -> status={st}, next_review={topic.get('next_review')}")

    save_tracker(tracker)
    print("Updated topic progress:")
    for line in changed:
        print(line)


def check_pure_c(lab_dir: Path) -> list[str]:
    errors: list[str] = []
    cpp_files = list(lab_dir.glob("*.cpp")) + list(lab_dir.glob("*.hpp")) + list(lab_dir.glob("*.cc"))
    if cpp_files:
        errors.append(f"Forbidden C++ files present: {[p.name for p in cpp_files]}")
    for fname in ("problem.h", "solution.c", "test_runner.c"):
        fpath = lab_dir / fname
        if not fpath.exists():
            errors.append(f"Missing required file: {fpath}")
            continue
        content = fpath.read_text(encoding="utf-8")
        # Strip C comments before scanning tokens to avoid false positives in Chinese/English comments
        stripped = re.sub(r"/\*.*?\*/", "", content, flags=re.DOTALL)
        stripped = re.sub(r"//.*", "", stripped)
        for pat, desc in CPP_FORBIDDEN_PATTERNS:
            if re.search(pat, stripped):
                errors.append(f"{fname}: detected forbidden {desc}")
    return errors


def cmd_clab(args: argparse.Namespace) -> None:
    dir_arg: Optional[str] = args.dir
    date_str = str(args.date)
    lab_dir = Path(dir_arg).resolve() if dir_arg else (CODE_ROOT / date_str)
    if args.clab_cmd == "init":
        lab_dir.mkdir(parents=True, exist_ok=True)
        mapping: dict[str, str] = {
            "problem.h.tpl": "problem.h",
            "solution.c.tpl": "solution.c",
            "test_runner.c.tpl": "test_runner.c",
        }
        for tpl_name, dst_name in mapping.items():
            tpl_path = TPL_DIR / tpl_name
            dst_path = lab_dir / dst_name
            if not tpl_path.exists():
                die(f"Missing template: {tpl_path}")
            if dst_path.exists() and not bool(args.force):
                die(f"{dst_path} already exists (use --force to overwrite)")
            dst_path.write_text(tpl_path.read_text(encoding="utf-8"), encoding="utf-8")
        print(f"Initialized pure-C lab scaffold at {lab_dir}")
        return

    # verify
    purity_errors = check_pure_c(lab_dir)
    if purity_errors:
        for e in purity_errors:
            print(f"[PURITY FAIL] {e}", file=sys.stderr)
        sys.exit(1)

    meta_path = lab_dir / ".lab_meta.json"
    test_file = lab_dir / "test_runner.c"
    expect = str(args.expect)
    if expect == "green" and meta_path.exists():
        meta: Any = json.loads(meta_path.read_text(encoding="utf-8"))
        expected_hash = meta.get("test_runner_sha256") if isinstance(meta, dict) else None
        if isinstance(expected_hash, str) and sha256_file(test_file) != expected_hash:
            die("Integrity check failed: test_runner.c was modified after lab generation!", code=3)

    runner_bin = lab_dir / "runner.out"
    compile_cmd: list[str] = [
        "cc",
        "-std=c11",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-Werror",
        str(lab_dir / "solution.c"),
        str(lab_dir / "test_runner.c"),
        "-o",
        str(runner_bin),
    ]
    comp = subprocess.run(compile_cmd, capture_output=True, text=True)
    if comp.returncode != 0:
        print("[COMPILE FAIL]", file=sys.stderr)
        print(comp.stderr, file=sys.stderr)
        sys.exit(1)

    run_res = subprocess.run([str(runner_bin)], capture_output=True, text=True)
    if runner_bin.exists():
        runner_bin.unlink()

    print(run_res.stdout, end="")
    if run_res.stderr:
        print(run_res.stderr, end="", file=sys.stderr)

    if expect == "red":
        if run_res.returncode == 0:
            die("Red-State Gate FAILED: starter stub unexpectedly passed all tests!", code=1)
        meta_path.write_text(
            json.dumps(
                {
                    "date": date_str,
                    "test_runner_sha256": sha256_file(test_file),
                    "verified_red_at": dt.datetime.now().isoformat(timespec="seconds"),
                },
                indent=2,
            )
            + "\n",
            encoding="utf-8",
        )
        print(
            f"[RED GATE OK] Starter stub compiles cleanly under C11 and fails tests as expected "
            f"(exit={run_res.returncode}). Recorded test_runner.c SHA-256."
        )
    elif expect == "green":
        if run_res.returncode != 0:
            die(f"Green-State Gate: tests failed (exit={run_res.returncode}).", code=1)
        print("[GREEN GATE OK] Pure-C11 compilation clean, integrity verified, all test cases passed.")
    else:
        sys.exit(run_res.returncode)


def cmd_validate(_: argparse.Namespace) -> None:
    errors: list[str] = []
    required_skill_files: list[Path] = [
        SKILL_DIR / "SKILL.md",
        SKILL_DIR / "references" / "question-design.md",
        SKILL_DIR / "references" / "c-lab-spec.md",
        SKILL_DIR / "references" / "grading-and-state.md",
        TPL_DIR / "problem.h.tpl",
        TPL_DIR / "solution.c.tpl",
        TPL_DIR / "test_runner.c.tpl",
        SKILL_DIR / "resources" / "daily_archive.md.tpl",
    ]
    for f in required_skill_files:
        if not f.exists():
            errors.append(f"Missing skill asset: {f}")

    tracker = load_tracker()
    seen_ids: set[str] = set()
    subjects: dict[str, Any] = tracker.get("subjects", {})
    for subj_code, subj in subjects.items():
        for t in subj.get("topics", []):
            tid = t.get("id")
            if not isinstance(tid, str) or not tid:
                errors.append(f"Missing topic id in {subj_code}")
                continue
            if tid in seen_ids:
                errors.append(f"Duplicate topic id: {tid}")
            seen_ids.add(tid)
            st = t.get("status")
            if not isinstance(st, str) or st not in VALID_STATUSES:
                errors.append(f"{tid}: invalid status '{st}'")
            for dkey in ("last_review", "next_review"):
                val = t.get(dkey)
                if val is not None:
                    if not isinstance(val, str):
                        errors.append(f"{tid}: {dkey} must be string or null, got {type(val).__name__}")
                    else:
                        try:
                            dt.date.fromisoformat(val)
                        except ValueError:
                            errors.append(f"{tid}: invalid {dkey} date '{val}'")

    if not MISTAKES_PATH.exists():
        errors.append(f"Missing mistakes file: {MISTAKES_PATH}")
    else:
        mtext = MISTAKES_PATH.read_text(encoding="utf-8")
        if "## 待变式回炉" not in mtext or "## 已攻克" not in mtext:
            errors.append("mistakes.md missing required '## 待变式回炉' or '## 已攻克' headings")

    if errors:
        for e in errors:
            print(f"[VALIDATE FAIL] {e}", file=sys.stderr)
        sys.exit(1)

    seal_n = int(tracker.get("config", {}).get("seal_latest_years", 5))
    allowed, sealed = get_exam_pools(seal_n)
    print(
        f"[VALIDATE OK] Skill assets complete; {len(seen_ids)} topics validated; "
        f"allowed_exams={len(allowed)}, sealed_exams={sealed}."
    )


HANDLERS: dict[str, Callable[[argparse.Namespace], None]] = {
    "status": cmd_status,
    "search": cmd_search,
    "read": cmd_read,
    "record": cmd_record,
    "progress": cmd_progress,
    "c-lab": cmd_clab,
    "validate": cmd_validate,
}


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description="408-daily-review deterministic CLI")
    sub = p.add_subparsers(dest="subcmd", required=True)

    p_status = sub.add_parser("status", help="Show seal boundary, due topics, and pending mistakes")
    p_status.add_argument("--today", default=dt.date.today().isoformat())

    p_search = sub.add_parser("search", help="Search keywords in non-sealed exam PDFs")
    p_search.add_argument("keywords", nargs="+")
    p_search.add_argument("--limit", type=int, default=6)

    p_read = sub.add_parser("read", help="Read line range from a non-sealed exam PDF")
    p_read.add_argument("year", type=int)
    p_read.add_argument("start", type=int)
    p_read.add_argument("end", type=int)

    p_rec = sub.add_parser("record", help="Record grading outcomes into tracker.json")
    p_rec.add_argument("--date", default=dt.date.today().isoformat())
    p_rec.add_argument("--topic", nargs="+", required=True, metavar="ID:OUTCOME")
    p_rec.add_argument("--used", nargs="*", metavar="YYYY-QN")
    p_rec.add_argument("--note", nargs="*", metavar="ID:TEXT")

    p_prog = sub.add_parser("progress", help="Update topic statuses in tracker.json")
    p_prog.add_argument("--set", dest="set_status", nargs="+", required=True, metavar="ID=STATUS")
    p_prog.add_argument("--today", default=dt.date.today().isoformat())

    p_clab = sub.add_parser("c-lab", help="Manage and verify pure-C11 coding labs")
    p_clab.add_argument("clab_cmd", choices=["init", "verify"])
    p_clab.add_argument("--date", default=dt.date.today().isoformat())
    p_clab.add_argument("--dir", help="Optional explicit lab directory override (e.g. for dry-run tests)")
    p_clab.add_argument("--expect", choices=["red", "green", "run"], default="run")
    p_clab.add_argument("--force", action="store_true")

    sub.add_parser("validate", help="Validate skill files and state schemas")

    return p


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()
    handler = HANDLERS.get(str(args.subcmd))
    if handler is None:
        die(f"Unknown subcommand: {args.subcmd}")
    handler(args)


if __name__ == "__main__":
    main()
