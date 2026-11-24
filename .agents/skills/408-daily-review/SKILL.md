---
name: 408-daily-review
description: >-
  Manages the user's daily 408 (Computer Science graduate entrance exam: DS, CO, OS, CN)
  spaced-repetition review loop, pure-C11 algorithm labs, answer grading, mistake tracking,
  and syllabus progress sync in this repository. Use whenever the user asks for daily or
  targeted 408 questions ("来今天的408", "408每日一练", "出几道题", "回炉错题"), asks for
  hints or testing on an active C coding problem ("卡住了", "帮我测一下"), submits answers
  for grading, or updates/queries their 408 study progress.
---

# 408 每日一练与间隔重复引擎

围绕仓库本地真题与学习账本，执行 `选题核验 → 防剧透作答 / 纯 C 实操 → 统考标准批改 → 间隔重复落盘` 闭环。

---

## 1. 全局红线与不变量（始终生效）

- **动态封存最新 5 年真题**：**严禁写死封存年份，严禁绕过脚本直接读取 `真题/408/*.pdf`**。题库检索与读取必须通过 [review_cli.py](./scripts/review_cli.py) 的 `status` / `search` / `read` 执行（代码层动态拦截最新 5 年 PDF）。
- **状态准入边界**：仅对 `learned`、`weak`、`learning` 考点出题；**严禁触碰 `skipped` 与 `not_started` 考点**。
- **写前零剧透 & 防代写护栏**：
  - 出题回合只给题干与作答指引，严禁附带答案、解析或标准实现。
  - 当存在进行中的纯 C 编程题（`408/review/code/YYYY-MM-DD/`）时，`solution.c` 对 Agent **只读不写**；遇到“报错了帮我修”“卡住了怎么写”等求助，拒绝代写并转入分层 Hint。
- **纯 C11 刚性标准**：第 4 题若为数据结构编程/算法题，**一律使用纯 C11（`.h` / `.c`），严禁任何 C++ 语法、引用 `&`、`new`/`delete` 或 STL**。出题交付前必须通过 `c-lab verify --expect red` 红灯门禁。
- **确定性核验与落盘**：计算/模拟题出题前 100% 经脚本独立验算；`tracker.json` 的间隔重复日期、掌握度与章节状态更新一律通过 `review_cli.py record` / `progress` 完成，并在落盘末尾运行 `review_cli.py validate`。

---

## 2. 意图路由与渐进式披露（按需加载）

处理请求前，仅按当前意图加载对应参考文档，不预读无关规范：

| 用户意图 | 典型触发表达 | 必调 CLI 子命令 | 按需读取参考文档 |
| :--- | :--- | :--- | :--- |
| **生成每日一练 / 专项出题** | “来今天的 408”“出几道图论基础题” | `status` → `search` / `read`（若含编程题加跑 `c-lab init` & `c-lab verify --expect red`） | [question-design.md](./references/question-design.md)；若第 4 题为编程题加读 [c-lab-spec.md](./references/c-lab-spec.md) |
| **纯 C 编程题求助 / 仅跑测** | “卡住了”“编译报错帮我看看”“先帮我跑下测试” | `c-lab verify --date YYYY-MM-DD --expect run` | [c-lab-spec.md](./references/c-lab-spec.md)（严格执行第 4 节三层 Hint，不改写 `solution.c`） |
| **提交作答与统考批改** | `1.A 2.C 3.B ...`、“写好了，批改吧” | （若含编程题先跑 `c-lab verify --expect green`）→ `record` → `validate` | [grading-and-state.md](./references/grading-and-state.md)；若含编程题加读 [c-lab-spec.md](./references/c-lab-spec.md) |
| **错题变式回炉** | “考考我之前的错题”“看看错题本” | `status`（确认待回炉列表与可用年份） | [question-design.md](./references/question-design.md)、[grading-and-state.md](./references/grading-and-state.md) |
| **同步学习进度 / 查账本** | “我把排序学完了”“解锁折半查找”“看看复习状态” | `progress` / `status` → `validate` | [grading-and-state.md](./references/grading-and-state.md) |

> 统一脚本入口：`python3 .agents/skills/408-daily-review/scripts/review_cli.py <subcmd> ...`

---

## 3. 目录契约与资产索引

```text
.agents/skills/408-daily-review/
├── SKILL.md                           # 意图路由、全局红线与目录契约（本文件）
├── scripts/
│   └── review_cli.py                  # 零依赖确定性引擎：status|search|read|record|progress|c-lab|validate
├── references/
│   ├── question-design.md             # 选题配比、5条质量硬标准、408教材分歧消歧表
│   ├── c-lab-spec.md                  # 纯C11规范、408标准结构体、Red/Green门禁、三层Hint、15分制量表
│   └── grading-and-state.md           # 批改输出结构、pass/weak/fail状态机规则、错题晋升与归档规范
└── resources/
    ├── daily_archive.md.tpl           # 每日归档 Markdown 模板
    └── c_lab_templates/               # 纯 C11 脚手架模板（problem.h.tpl / solution.c.tpl / test_runner.c.tpl）

408/review/                            # 仓库持久化学习状态数据区
├── tracker.json                       # 考点状态、掌握度、间隔重复日期与已用真题记录
├── mistakes.md                        # 待变式回炉 / 已攻克错题账本
├── daily/YYYY-MM-DD.md                # 每日题目、作答与精讲归档
└── code/YYYY-MM-DD/                   # 纯 C 算法大题工作目录（runner.out 由 .gitignore 自动忽略）
```
