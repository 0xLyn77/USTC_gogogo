---
name: 408-daily-review
description: >-
  Runs the user's daily 408 (Computer Science graduate entrance exam: DS, CO, OS, CN)
  spaced-repetition quiz, grading, mistake tracking, and syllabus progress sync in this
  repository. Use whenever the user asks for daily 408 practice/questions ("来今天的408",
  "408每日一练", "出几道408题", "复习408"), submits answers for grading, asks to review
  mistakes, or updates their 408 study progress/unlocked chapters.
---

# 408 每日一练与间隔重复复习工作流

本 Skill 负责在当前仓库中持续维护一套高质量、可追踪、按遗忘曲线调度的 408 每日复习闭环。

## 核心数据文件

- 进度与间隔重复账本：`408/review/tracker.json`
- 错题与坑点本：`408/review/mistakes.md`
- 每日练习归档：`408/review/daily/YYYY-MM-DD.md`
- 本地真题库：`真题/408/*.pdf`
- 辅助脚本（**强制用于动态 5 年封存校验与选题**）：[exam_pool.py](./scripts/exam_pool.py)

---

## 铁律（出题与批改必须严格遵守）

### 1. 动态封存最新 5 年真题（绝不写死年份）
- **永远不要在规则或代码里写死具体封存年份**。
- 每次选题前必须运行：
  ```bash
  python3 .agents/skills/408-daily-review/scripts/exam_pool.py --status
  ```
- 脚本会动态扫描 `真题/408/*.pdf`，自动把**最新的 5 个年份**列为封存区（`sealed`），仅允许从其余较早年份（`allowed`）抽题。
- **严禁读取、搜索或引用任何封存年份的 PDF**，一律通过 `exam_pool.py --search` / `--read` 访问题库（脚本在代码层面拒绝访问封存年份）。

### 2. 题量、选题配比与题源结构
- **每日默认题量**：**3 道单选题 + 1 道大题**（简答 / 手算模拟 / 算法设计或改错），预计耗时 10–15 分钟（除非用户临时指定专项或题量）。
- **只出已解锁考点**：
  - 允许出题的 `status`：`learned`、`weak`、`learning`。
  - **绝对不出题的 `status`**：`skipped`、`not_started`（除非用户明确说已学完并要求解锁）。
  - 对 `weak` 且标注“遗忘/潦草学过”的章节（如图论），优先从基础概念与小规模手算题温和回炉，不要一上来就上偏难综合题。
- **选题配比**（跨天长期近似满足）：
  - **60% 到期旧知识**：优先抽 `next_review <= 今日` 或 `mastery` 较低的 `learned`/`weak` 考点。
  - **20% 历史错题变式**：从 `408/review/mistakes.md` 的「待变式回炉」里挑一条，改数据/换条件/换视角出变式题检验是否真掌握。
  - **20% 近期新学知识**：从 `status == "learning"` 或最近 `diary/` / `408/` 新记的内容中抽题趁热巩固。
- **题源结构**：
  - **约 70% 早期真题（原题或同构改编）**：优先从未出现在 `tracker.json` 的 `used_real_questions` 中的早期真题抽取。
  - **约 30% 针对性补充题**：针对错题坑点或用户仓库 `408/` 下自己写的代码/笔记出追问、变式或改错题。

### 3. 五条质量硬标准
1. **来源透明**：每道题标题必须显式标注 `[真题原题 · YYYY年 第X题]`、`[真题变式 · 参考YYYY年 第X题]` 或 `[考点自编 · 考点名]`，绝不拿自编题冒充真题。
2. **口径对齐考纲、无歧义**：严格卡在 408 考纲内；遇到不同教材存在口径分歧的概念（如 B 树高度是否含失败结点、数组起始下标 0 还是 1、循环队列指针含义等），题干必须显式给出定义。
3. **100% 程序验算**：凡涉及数值计算、矩阵下标映射、树/图遍历与最短路/MST/拓扑/AOE、排序趟数、进制/IEEE754/Cache/页表、PV/调度、子网划分/TCP 窗口或代码追踪的题，**出题前必须写 Python/C++ 脚本跑出唯一正解**，绝不靠心算。
4. **干扰项有真实错因**：选择题的每个错误选项必须对应真实常见误区，拒绝“一眼假”凑数项或文字游戏。
5. **定位清晰**：日常每日一练定位为“反遗忘 + 查漏补缺 + 保持手算手感”，近 5 年整套真题留给后期全真限时模考。

### 4. 先隐藏答案
- 出题回合**绝不提前剧透答案、解析或明显提示**。等用户作答后再逐题批改。

---

## 流程 A：生成今日练习

1. **查看状态与可用题池**：
   ```bash
   python3 .agents/skills/408-daily-review/scripts/exam_pool.py --status
   ```
   同时阅读 `408/review/mistakes.md` 中的「待变式回炉」条目，必要时扫一眼最近 1~2 天的 `diary/` 确认是否有刚学的新章节。
2. **确定今日 4 题组合**（3 选择 + 1 大题），兼顾到期旧知识、错题变式与近期新学内容。
3. **从允许年份检索真题**：
   ```bash
   python3 .agents/skills/408-daily-review/scripts/exam_pool.py --search 关键词1 关键词2
   python3 .agents/skills/408-daily-review/scripts/exam_pool.py --read <允许年份> <起始行> <结束行>
   ```
4. **编写验算脚本并运行**：验证所有计算/模拟/代码题的唯一正解与干扰项合理性。
5. **向用户呈现题目**（不含答案），提示用户按 `1.X 2.X 3.X` + 第 4 题思路/代码直接回复。

---

## 流程 B：批改、拆解与状态落盘

用户提交答案后，依次完成：

1. **逐题批改与拆坑点**：
   - 给出对错总评。
   - 对错题或“蒙对/不确定的题”，必须给出**可复用的推导步骤**、**错因标签**，并点破真题干扰项的设计意图。
   - 对大题，肯定正确思路并指出 408 评分细节（边界判空、时空复杂度、代码规范）。
2. **更新 `408/review/tracker.json`**：
   - 把本次用到的真题原题编号（如 `2016-Q4`）追加到 `used_real_questions`。
   - 按表现更新涉及考点的间隔重复参数（`spaced_intervals_days = [1, 3, 7, 15, 30]`）：
     - **答对且扎实**：`mastery = min(5, mastery + 1)`，`interval_idx = min(4, interval_idx + 1)`，`next_review = 今日 + intervals[interval_idx]`；若原为 `weak` 且 `mastery >= 3`，可转为 `learned`。
     - **答错 / 蒙对不熟**：`mastery = max(1, mastery - 1)`，`interval_idx = 0`，`next_review = 今日 + 1 天`，`status = "weak"`，更新 `notes`。
   - 更新 `last_review = 今日`。
3. **更新 `408/review/mistakes.md`**：
   - 新增本次错题或“蒙对不熟”条目到「待变式回炉」。
   - 若某条旧错题的变式题本次完全做对且思路清晰，将其移入「已攻克」并记录攻克日期。
4. **写入今日归档 `408/review/daily/YYYY-MM-DD.md`**：
   - 记录题目（含来源标签）、用户作答、参考答案与核心解析、错因小结。若当天已有练习文件，追加为新小节（如 `## 练习 2`）。

---

## 流程 C：同步学习进度（解锁/跳过章节）

当用户说“我把折半查找学完了”“计组开始看数据表示了”或“XX 先跳过”时：
1. 直接修改 `408/review/tracker.json` 对应考点的 `status`（`not_started` / `skipped` / `learning` / `learned` / `weak`）与 `next_review`。
2. 简要反馈已解锁/调整的考点范围。
