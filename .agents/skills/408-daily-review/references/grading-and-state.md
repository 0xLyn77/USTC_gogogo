# 批改契约、间隔重复状态机与落盘规范

当用户提交答案请求批改、复盘错题或同步章节进度时加载本文档。

---

## 1. 批改回复结构

1. **总评速览**：一行列出各题结果（如：`1 对 · 2 错 · 3 对（待固推导）· 4 通过（14/15 分）`）。
2. **选择题逐题拆解**：
   - 给出独立简洁的推导步骤（优先给通用三步法/表格法，少让用户死背公式）。
   - 对错题或不确定题，标注**错因标签**，并说明被选错误选项与其余典型干扰项各自对应的误区。
3. **大题点评**：
   - 若为纯 C 编程题：先展示 `c-lab verify` 测试结果，再按 `references/c-lab-spec.md` 的 **15 分三段式量表**逐项打分，最后附上精炼的考场满分参考 C 代码供对比。
   - 若为非编程大题：逐步核验手算中间状态（如每趟 `dist[]` / 拓扑序列 / `ve, vl`），指出考场采分点。

---

## 2. 确定性状态落盘（严禁手改 JSON 日期）

批改完成后，**必须调用 `review_cli.py` 更新 `408/review/tracker.json`**，避免手动计算日期或破坏 Schema：

```bash
python3 .agents/skills/408-daily-review/scripts/review_cli.py record \
  --date YYYY-MM-DD \
  --topic <topic_id_1>:<pass|weak|fail> <topic_id_2>:<pass|weak|fail> \
  --used <YYYY-QN> \
  --note "<topic_id>:<简短复盘备注>"
```

### 结果档位判定准则（`pass` / `weak` / `fail`）
- `pass`：独立答对且推理清晰（或 C 编程题一次/轻微修正拿到最优档 $\ge 13/15$ 分）。状态机自动提升 `mastery`（上限 5）、推进间隔档位 `[1, 3, 7, 15, 30]`，若 `mastery >= 3` 自动将 `weak`/`learning` 晋升为 `learned`。
- `weak`：结果蒙对但原理模糊，或编程题写出暴力次优解/边界漏判较多（$9\sim 12/15$ 分）。保持 `mastery >= 2`，重置间隔为 `+1 天`，标记 `status = "weak"`。
- `fail`：概念错、计算错或算法逻辑不成立。下调 `mastery`（下限 1），重置间隔为 `+1 天`，标记 `status = "weak"`。

> 注意：`--used` 仅传入本次使用到的**真题原题**编号（格式 `YYYY-QN`，如 `2018-Q3`）；变式题和自编题不计入 `--used`。

---

## 3. 错题本 `408/review/mistakes.md` 维护规则

1. **新增错题**：凡判定为 `fail` 或 `weak`（含蒙对不熟）的题目，在 `## 待变式回炉` 下按递增编号 `M00X` 追加条目，包含：`日期`、`来源`、`考点 ID`、`状态`、`错因标签`、`核心防坑点`。
2. **变式攻克晋升**：当某道变式回炉题被判定为 `pass` 时，将原 `M00X` 条目从 `## 待变式回炉` 剪切移入 `## 已攻克`，并补充 `- **攻克记录**：YYYY-MM-DD 变式题独立通过`。

---

## 4. 每日归档与进度同步

- **归档文件**：参照 `resources/daily_archive.md.tpl` 写入 `408/review/daily/YYYY-MM-DD.md`（若当日已有练习，追加 `## 加练 N`）。
- **章节解锁/跳过**：当用户汇报新进度时，调用：
  ```bash
  python3 .agents/skills/408-daily-review/scripts/review_cli.py progress \
    --today YYYY-MM-DD \
    --set <topic_id>=<learning|learned|weak|skipped|not_started>
  ```
- **收尾自检**：每次修改完状态文件后，顺手在同一条命令链末尾运行 `review_cli.py validate` 确保账本与错题本格式零损坏。
