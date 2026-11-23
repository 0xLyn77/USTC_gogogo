#!/usr/bin/env python3
"""Helper for 408-daily-review skill.

Enforces the dynamic latest-N-years seal rule (never hardcoded) on 真题/408/*.pdf,
reports due/weak/learning topics from 408/review/tracker.json, and safely searches
or reads ONLY allowed (non-sealed) exam PDFs.
"""

import argparse
import datetime as dt
import json
import re
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[4]
EXAM_DIR = REPO_ROOT / "真题" / "408"
TRACKER_PATH = REPO_ROOT / "408" / "review" / "tracker.json"


def load_tracker() -> dict:
    if not TRACKER_PATH.exists():
        print(f"ERROR: tracker not found at {TRACKER_PATH}", file=sys.stderr)
        sys.exit(1)
    return json.loads(TRACKER_PATH.read_text(encoding="utf-8"))


def get_exam_pools(seal_count: int) -> tuple[list[int], list[int]]:
    if not EXAM_DIR.exists():
        return [], []
    years = []
    for p in EXAM_DIR.glob("*.pdf"):
        m = re.fullmatch(r"(\d{4})", p.stem)
        if m:
            years.append(int(m.group(1)))
    years.sort()
    if seal_count <= 0:
        return years, []
    if len(years) <= seal_count:
        return [], years
    return years[:-seal_count], years[-seal_count:]


def pdf_lines(year: int) -> list[str]:
    p = EXAM_DIR / f"{year}.pdf"
    if not p.exists():
        print(f"ERROR: PDF not found for year {year}: {p}", file=sys.stderr)
        sys.exit(1)
    out = subprocess.check_output(
        ["pdftotext", str(p), "-"], stderr=subprocess.DEVNULL
    ).decode("utf-8", errors="ignore")
    return out.splitlines()


def cmd_status(tracker: dict, today_str: str) -> None:
    seal_n = int(tracker.get("config", {}).get("seal_latest_years", 5))
    allowed, sealed = get_exam_pools(seal_n)
    quota = tracker.get("config", {}).get("daily_quota", {"choice": 3, "free_response": 1})

    print(f"=== 408 Daily Review Status ({today_str}) ===")
    print(f"Daily quota: {quota.get('choice', 3)} choice + {quota.get('free_response', 1)} free-response")
    print(f"Dynamic seal rule: latest {seal_n} years")
    print(f"  SEALED (DO NOT READ): {sealed}")
    print(f"  ALLOWED ({len(allowed)} years): {allowed}")
    print(f"  Used real questions ({len(tracker.get('used_real_questions', []))}): {tracker.get('used_real_questions', [])}")

    due, weak, learning, skipped = [], [], [], []
    for subj_code, subj in tracker.get("subjects", {}).items():
        for t in subj.get("topics", []):
            st = t.get("status", "not_started")
            nr = t.get("next_review")
            entry = (subj_code, t["id"], t["name"], st, t.get("mastery", 0), nr, t.get("notes", ""))
            if st == "skipped":
                skipped.append(entry)
            elif st == "learning":
                learning.append(entry)
            elif st == "weak":
                weak.append(entry)
                if nr and nr <= today_str:
                    due.append(entry)
            elif st == "learned" and nr and nr <= today_str:
                due.append(entry)

    def fmt_list(title: str, items: list) -> None:
        print(f"\n--- {title} ({len(items)}) ---")
        if not items:
            print("  (none)")
            return
        for subj_code, tid, name, st, mastery, nr, notes in items:
            note_s = f" | {notes}" if notes else ""
            print(f"  [{subj_code}] {tid} ({name}) | status={st} mastery={mastery} next={nr}{note_s}")

    fmt_list("DUE TODAY (next_review <= today)", due)
    fmt_list("WEAK / NEEDS GENTLE REVIEW", weak)
    fmt_list("RECENT LEARNING", learning)
    fmt_list("SKIPPED (DO NOT QUIZ)", skipped)


def cmd_search(tracker: dict, keywords: list[str], limit_per_year: int) -> None:
    seal_n = int(tracker.get("config", {}).get("seal_latest_years", 5))
    allowed, sealed = get_exam_pools(seal_n)
    print(f"[Seal enforced] Searching ONLY allowed years {allowed}; sealed={sealed}")
    for y in allowed:
        lines = pdf_lines(y)
        hits = []
        for idx, line in enumerate(lines):
            if any(kw.lower() in line.lower() for kw in keywords):
                hits.append((idx, line.strip()))
        if hits:
            print(f"\n=== {y} ({len(hits)} hits) ===")
            for idx, txt in hits[:limit_per_year]:
                print(f"  L{idx}: {txt}")


def cmd_read(tracker: dict, year: int, start: int, end: int) -> None:
    seal_n = int(tracker.get("config", {}).get("seal_latest_years", 5))
    allowed, sealed = get_exam_pools(seal_n)
    if year in sealed:
        print(
            f"BLOCKED: Year {year} is in the dynamically sealed latest {seal_n} years {sealed}. "
            "Refusing to read to prevent spoiling mock exams.",
            file=sys.stderr,
        )
        sys.exit(2)
    if year not in allowed:
        print(f"ERROR: Year {year} not found in allowed pool {allowed}.", file=sys.stderr)
        sys.exit(1)
    lines = pdf_lines(year)
    start = max(0, start)
    end = min(len(lines), end)
    print(f"=== {year} L{start}-{end} (allowed pool) ===")
    for i in range(start, end):
        print(f"{i:4d}: {lines[i]}")


def main() -> None:
    parser = argparse.ArgumentParser(description="408 daily review exam pool & tracker helper")
    parser.add_argument("--status", action="store_true", help="Show seal boundary and due topics")
    parser.add_argument("--today", default=dt.date.today().isoformat(), help="Override today YYYY-MM-DD")
    parser.add_argument("--search", nargs="+", metavar="KW", help="Search keywords in allowed PDFs only")
    parser.add_argument("--limit", type=int, default=6, help="Max hits per year for --search")
    parser.add_argument("--read", nargs=3, type=int, metavar=("YEAR", "START", "END"), help="Read line range from an allowed PDF")
    args = parser.parse_args()

    tracker = load_tracker()
    if args.status:
        cmd_status(tracker, args.today)
    elif args.search:
        cmd_search(tracker, args.search, args.limit)
    elif args.read:
        cmd_read(tracker, args.read[0], args.read[1], args.read[2])
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
