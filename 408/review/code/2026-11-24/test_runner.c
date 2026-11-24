#include "problem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_total = 0;
static int g_passed = 0;

#define CHECK(cond, name, detail_fmt, ...)                                      \
    do {                                                                        \
        g_total++;                                                              \
        if (cond) {                                                             \
            g_passed++;                                                         \
            printf("[PASS] Case %d: %s\n", g_total, (name));                    \
        } else {                                                                \
            printf("[FAIL] Case %d: %s -> " detail_fmt "\n", g_total, (name),   \
                   __VA_ARGS__);                                                \
        }                                                                       \
    } while (0)

static LinkList build_list(const int *vals, int len, LNode **out_nodes) {
    LinkList head = (LinkList)malloc(sizeof(LNode));
    LNode *tail = head;
    int i;
    head->data = 0;
    head->next = NULL;
    for (i = 0; i < len; i++) {
        LNode *node = (LNode *)malloc(sizeof(LNode));
        node->data = vals[i];
        node->next = NULL;
        tail->next = node;
        tail = node;
        if (out_nodes != NULL) {
            out_nodes[i] = node;
        }
    }
    return head;
}

static void format_list(LinkList head, int max_steps, char *buf, size_t cap) {
    size_t pos = 0;
    int steps = 0;
    LNode *p = (head != NULL) ? head->next : NULL;
    pos += (size_t)snprintf(buf + pos, cap - pos, "[");
    while (p != NULL && steps < max_steps && pos + 16 < cap) {
        pos += (size_t)snprintf(buf + pos, cap - pos, "%s%d",
                                (steps == 0) ? "" : ", ", p->data);
        p = p->next;
        steps++;
    }
    if (p != NULL) {
        snprintf(buf + pos, cap - pos, ", ...(cycle/unterminated)]");
    } else {
        snprintf(buf + pos, cap - pos, "]");
    }
}

static bool match_expected(LinkList head, const int *exp, int exp_len,
                           int max_steps, bool *out_safe_to_free) {
    int idx = 0;
    int steps = 0;
    LNode *p;
    if (head == NULL) {
        *out_safe_to_free = false;
        return false;
    }
    p = head->next;
    while (p != NULL && steps <= max_steps) {
        if (idx >= exp_len || p->data != exp[idx]) {
            /* Continue walking up to max_steps to check termination safety */
            while (p != NULL && steps <= max_steps) {
                p = p->next;
                steps++;
            }
            *out_safe_to_free = (p == NULL);
            return false;
        }
        idx++;
        p = p->next;
        steps++;
    }
    if (p != NULL) {
        *out_safe_to_free = false;
        return false;
    }
    *out_safe_to_free = true;
    return (idx == exp_len);
}

static void free_list_safe(LinkList head, bool safe) {
    LNode *p;
    if (!safe || head == NULL) {
        return;
    }
    p = head;
    while (p != NULL) {
        LNode *nxt = p->next;
        free(p);
        p = nxt;
    }
}

static void run_seq_case(const char *case_name, const int *in_vals, int in_len,
                         int n, const int *exp_vals, int exp_len,
                         const char *exp_str) {
    LinkList head = build_list(in_vals, in_len, NULL);
    bool safe_free = false;
    bool ok;
    char got_buf[256];

    purge_dup_abs(head, n);
    format_list(head, in_len + 3, got_buf, sizeof(got_buf));
    ok = match_expected(head, exp_vals, exp_len, in_len + 3, &safe_free);
    CHECK(ok, case_name, "expected %s, got %s", exp_str, got_buf);
    free_list_safe(head, safe_free);
}

int main(void) {
    printf("=== 408 Pure-C Algorithm Test Runner (2015-Q41) ===\n");

    /* Part A: 常规用例 */
    {
        const int in1[] = {21, -15, -15, -7, 15};
        const int exp1[] = {21, -15, -7};
        run_seq_case("2015真题原题样例（含相邻重复与正负同绝对值）",
                     in1, 5, 21, exp1, 3, "[21, -15, -7]");
    }
    {
        const int in2[] = {3, -1, 2, 1, -3, -2, 4, -4};
        const int exp2[] = {3, -1, 2, 4};
        run_seq_case("多组正负交错重复（验证保留首次出现符号）",
                     in2, 8, 5, exp2, 4, "[3, -1, 2, 4]");
    }

    /* Part B: 408 高频扣分边界 */
    {
        LinkList empty_head = build_list(NULL, 0, NULL);
        bool safe_free = false;
        bool ok;
        purge_dup_abs(NULL, 5);
        purge_dup_abs(empty_head, 5);
        ok = match_expected(empty_head, NULL, 0, 2, &safe_free);
        CHECK(ok, "NULL指针与仅含头结点的空表安全边界",
              "expected [], head->next=%p", (void *)empty_head->next);
        free_list_safe(empty_head, safe_free);
    }
    {
        const int in4[] = {-8};
        const int exp4[] = {-8};
        run_seq_case("单元素链表边界", in4, 1, 8, exp4, 1, "[-8]");
    }
    {
        const int in5[] = {0, -10, 10, 0, 5, -10};
        const int exp5[] = {0, -10, 5};
        run_seq_case("含 0 与极值 +-n 边界（检验辅助空间下标上界 n）",
                     in5, 6, 10, exp5, 3, "[0, -10, 5]");
    }
    {
        const int in6[] = {-6, 6, -6, 6};
        const int exp6[] = {-6};
        run_seq_case("全表绝对值相等（连续删除至表尾封口）",
                     in6, 4, 6, exp6, 1, "[-6]");
    }

    /* Part C: 结构安全与原结点复用校验 */
    {
        const int in7[] = {4, -4, 2, 9, -2};
        const int exp7[] = {4, 2, 9};
        LNode *orig_nodes[5] = {NULL, NULL, NULL, NULL, NULL};
        const LNode *kept_ptrs[3];
        LinkList head = build_list(in7, 5, orig_nodes);
        bool safe_free = false;
        bool seq_ok;
        bool ptr_reused = true;
        char got_buf[256];

        kept_ptrs[0] = orig_nodes[0];
        kept_ptrs[1] = orig_nodes[2];
        kept_ptrs[2] = orig_nodes[3];

        purge_dup_abs(head, 9);
        format_list(head, 8, got_buf, sizeof(got_buf));
        seq_ok = match_expected(head, exp7, 3, 8, &safe_free);
        if (seq_ok) {
            LNode *cur = head->next;
            int k;
            for (k = 0; k < 3; k++) {
                if (cur != kept_ptrs[k]) {
                    ptr_reused = false;
                    break;
                }
                cur = cur->next;
            }
            if (cur != NULL) {
                ptr_reused = false;
            }
        } else {
            ptr_reused = false;
        }
        CHECK(seq_ok && ptr_reused,
              "原地保留首次出现结点地址且尾结点 next==NULL",
              "got=%s, inplace_reuse=%s", got_buf,
              ptr_reused ? "true" : "false");
        free_list_safe(head, safe_free);
    }

    printf("----------------------------------------\n");
    printf("Result: %d / %d passed\n", g_passed, g_total);
    return (g_passed == g_total && g_total > 0) ? 0 : 1;
}
