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

static void format_arr(const int *arr, int n, char *buf, size_t cap) {
    size_t pos = 0;
    int i;
    pos += (size_t)snprintf(buf + pos, cap - pos, "[");
    for (i = 0; i < n && pos + 16 < cap; i++) {
        pos += (size_t)snprintf(buf + pos, cap - pos, "%s%d",
                                (i == 0) ? "" : ", ", arr[i]);
    }
    snprintf(buf + pos, cap - pos, "]");
}

static void run_array_case(const char *case_name, const int *init_vals, int n,
                           int p, const int *exp_vals) {
    int buf[32];
    char got_buf[256];
    char exp_buf[256];
    bool ok = true;
    int i;

    memcpy(buf, init_vals, (size_t)n * sizeof(int));
    left_shift(buf, n, p);

    for (i = 0; i < n; i++) {
        if (buf[i] != exp_vals[i]) {
            ok = false;
            break;
        }
    }
    format_arr(buf, n, got_buf, sizeof(got_buf));
    format_arr(exp_vals, n, exp_buf, sizeof(exp_buf));
    CHECK(ok, case_name, "expected %s, got %s", exp_buf, got_buf);
}

int main(void) {
    printf("=== 408 Pure-C Algorithm Test Runner (2010-Q42) ===\n");

    /* Part A: 常规用例 */
    {
        const int in1[] = {1, 2, 3, 4, 5, 6, 7};
        const int exp1[] = {4, 5, 6, 7, 1, 2, 3};
        run_array_case("常规循环左移（n=7, p=3）", in1, 7, 3, exp1);
    }
    {
        const int in2[] = {-5, 0, 5, -5, 10, 2};
        const int exp2[] = {5, -5, 10, 2, -5, 0};
        run_array_case("含负数与重复元素（n=6, p=2）", in2, 6, 2, exp2);
    }

    /* Part B: 408 高频扣分边界 */
    {
        const int in3[] = {10, 20, 30, 40, 50};
        const int exp3[] = {20, 30, 40, 50, 10};
        run_array_case("极小左移步长（n=5, p=1）", in3, 5, 1, exp3);
    }
    {
        const int in4[] = {10, 20, 30, 40, 50};
        const int exp4[] = {50, 10, 20, 30, 40};
        run_array_case("极大左移步长（n=5, p=n-1=4）", in4, 5, 4, exp4);
    }
    {
        const int in5[] = {7, 8};
        const int exp5[] = {8, 7};
        run_array_case("最小合法数组（n=2, p=1）", in5, 2, 1, exp5);
    }
    {
        int single[] = {42};
        int four[] = {1, 2, 3, 4};
        bool ok = true;
        left_shift(NULL, 5, 2);
        left_shift(single, 1, 0);
        left_shift(four, 4, 0);
        if (single[0] != 42 || four[0] != 1 || four[1] != 2 || four[2] != 3 ||
            four[3] != 4) {
            ok = false;
        }
        CHECK(ok, "NULL指针、单元素与 p=0 安全边界",
              "single[0]=%d, four=[%d,%d,%d,%d]", single[0], four[0], four[1],
              four[2], four[3]);
    }

    /* Part C: 结构安全（前后哨兵防越界写） */
    {
        const int sentinel = 0x5A5A5A5A;
        int buf[10] = {sentinel, 9, 8, 7, 6, 5, 4, 3, 2, sentinel};
        const int exp7[] = {4, 3, 2, 9, 8, 7, 6, 5};
        bool data_ok = true;
        bool guard_ok;
        char got_buf[256];
        int i;

        left_shift(buf + 1, 8, 5);
        for (i = 0; i < 8; i++) {
            if (buf[i + 1] != exp7[i]) {
                data_ok = false;
                break;
            }
        }
        guard_ok = (buf[0] == sentinel && buf[9] == sentinel);
        format_arr(buf + 1, 8, got_buf, sizeof(got_buf));
        CHECK(data_ok && guard_ok, "前后哨兵越界防护（n=8, p=5）",
              "got=%s, guard_ok=%s", got_buf, guard_ok ? "true" : "false");
    }

    printf("----------------------------------------\n");
    printf("Result: %d / %d passed\n", g_passed, g_total);
    return (g_passed == g_total && g_total > 0) ? 0 : 1;
}
