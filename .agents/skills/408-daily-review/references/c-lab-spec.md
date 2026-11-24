# 纯 C 语言实操算法题规范（408 第 41 题标准）

当第 4 题为数据结构编程/算法题，或用户对当前代码实验室（`408/review/code/YYYY-MM-DD/`）请求提示、测试、批改时加载本文档。

---

## 1. 语言与考场规范红线

1. **纯 C11 强制约束**：仅允许 `.h` 与 `.c` 文件，统一由 `cc -std=c11 -Wall -Wextra -Wpedantic -Werror` 编译。
2. **禁用一切 C++ 语法**：严禁 `.cpp`/`.hpp`、引用传参（`SqList &L` / `LinkList &L`）、`new`/`delete`、`bool` 以外的 C++ 关键字、`std::*` 或任何 STL 容器/算法。
   - 需在函数内修改结构体或头指针时，在纯 C 中统一传**指针**（如 `SqList *L`、`LinkList *L`）或将新头指针作为返回值，并在 `problem.h` 注释写清接口契约（包括是否带头结点 `dummy head`）。
3. **防代写护栏**：
   - 实验室进行中，`solution.c` 对 Agent **只读不写**（仅在出题初始化阶段生成空骨架）。
   - 用户说“报错了帮我改”“卡住了怎么写”时，**严禁直接给出完整可运行 C 代码**，必须按第 4 节的分层 Hint 协议引导。

---

## 2. 408 统考标准纯 C 结构体速查（用于 `problem.h`）

按题目所属章节选用与统考卷面一致的类型命名：

```c
/* 1. 顺序表 */
#define MAXSIZE 100
typedef struct {
    int data[MAXSIZE];
    int length;
} SqList;

/* 2. 单链表（必须在注释注明：是否带头结点！） */
typedef struct LNode {
    int data;
    struct LNode *next;
} LNode, *LinkList;

/* 3. 二叉树链式存储 */
typedef struct BiTNode {
    int data;
    struct BiTNode *lchild, *rchild;
} BiTNode, *BiTree;

/* 4. 图 - 邻接矩阵 */
#define MAXV 50
typedef struct {
    int numVertices, numEdges;
    char VerticesList[MAXV];
    int Edge[MAXV][MAXV];
} MGraph;

/* 5. 图 - 邻接表 */
typedef struct ArcNode {
    int adjvex;
    int weight;
    struct ArcNode *nextarc;
} ArcNode;
typedef struct VNode {
    char data;
    ArcNode *firstarc;
} VNode, AdjList[MAXV];
typedef struct {
    AdjList vertices;
    int vexnum, arcnum;
} ALGraph;
```

---

## 3. 生成流程与 Red-State 门禁（出题必过）

1. 初始化目录：
   ```bash
   python3 .agents/skills/408-daily-review/scripts/review_cli.py c-lab init --date YYYY-MM-DD
   ```
2. 编辑 `problem.h`（结构与函数契约）与 `test_runner.c`：
   - **防剧透原则**：`test_runner.c` 用预计算期望序列或不变量校验（如链表节点地址集未泄漏、有序性、长度核验），**绝不在 `test_runner.c` 里写标准最优解源码**。
   - **分层用例（5–8 组）**：
     - Part A（常规用例）：2–3 组典型输入；
     - Part B（408 高频扣分边界）：空表/空树/`NULL`、单元素、首元/尾元命中、全命中或全不命中、含负数/重复值；
     - Part C（结构安全）：检查尾结点 `next == NULL`（防野指针死循环）、原地重排题检查原结点地址复用情况。
3. 先用临时参考实现验证 `test_runner.c` 能跑通 Green，随后将 `solution.c` 还原为带占位返回值的空骨架，并执行 **Red-State 门禁**：
   ```bash
   python3 .agents/skills/408-daily-review/scripts/review_cli.py c-lab verify --date YYYY-MM-DD --expect red
   ```
   确认输出 `[RED GATE OK]`（自动校验无 C++ 混入、C11 零警告编译通过、空骨架按预期失败、记录 `test_runner.c` SHA-256 防篡改摘要并自动清理 `runner.out`）后，方可向用户发题。

---

## 4. 防剧透三层 Hint 协议（当用户求助时）

- **Level 1（不变量与边界追问）**：指出编译错误行或挂掉的那组测试输入/输出差异，用一个问题点出漏掉的边界（如“当删除的恰好是最后一个结点时，`prev->next` 指向了哪里？”）。
- **Level 2（指针/下标状态图）**：用 ASCII 图画出断链/重连前后的指针指向顺序或双指针移动条件，不给 C 语句。
- **Level 3（局部伪代码框架）**：仅给出卡点处 3–5 行伪代码步骤，把核心条件表达式留给用户补全。

---

## 5. 408 第 41 题 15 分制批改量表

运行 `review_cli.py c-lab verify --date YYYY-MM-DD --expect green` 后，读取 `solution.c` 按统考三段式给分：

1. **算法基本设计思想（4 分）**：检查 `solution.c` 顶部的 `[设计思想]` 注释是否清晰说明核心策略（如快慢指针、头插逆置、辅助计数数组）；若暴力解法方向仅得 1–2 分。
2. **纯 C 代码实现与边界健壮性（9 分）**：
   - 最优复杂度档满分基准 9 分；次优/暴力档上限 5–6 分（对齐 408 历年阅卷档位）。
   - 扣分项检查：未判空指针/空表（-1）、断链前未暂存后继导致断链或野指针（-2~-3）、新表尾未置 `NULL`（-2）、动态 `malloc`/`calloc` 辅助数组未 `free`（-1）。
3. **时间与空间复杂度说明（2 分）**：检查顶部 `[复杂度]` 注释是否与实际所写代码一致（各 1 分；若代码实为 `O(n^2)` 却写成 `O(n)` 不得分并指出原因）。
