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

int main(void) {
    printf("=== 408 Pure-C Algorithm Test Runner ===\n");

    /* Part A: 常规用例 */
    SqList a = {{1, 2, 3}, 3};
    bool ok = solve(&a);
    CHECK(ok == true, "基础常规用例", "expected ret=true, got ret=%s",
          ok ? "true" : "false");

    /* Part B: 408 高频扣分边界（空表、单元素、首尾命中等） */

    printf("----------------------------------------\n");
    printf("Result: %d / %d passed\n", g_passed, g_total);
    return (g_passed == g_total && g_total > 0) ? 0 : 1;
}
