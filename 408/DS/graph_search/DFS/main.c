#include <stdio.h>
#include <string.h>

#include "graph.h"

static void PrintOrder(const char* tag, const VertexType order[],
                       int order_count) {
    int i; /* i：遍历访问次序数组的循环变量。 */

    /* tag：输出行标签；order：访问次序；order_count：访问次序长度。 */
    printf("%s\t", tag);
    for (i = 0; i < order_count; ++i) {
        printf(" %c", order[i]);
    }
    putchar('\n');
}

static void PrintResult(const VertexType user_order[], int user_count) {
    const VertexType* want_order; /* want_order：指向标准 DFS 次序数组。 */
    int want_count;               /* want_count：标准 DFS 次序的顶点个数。 */

    /* user_order：用户访问次序；user_count：用户访问次序长度。 */
    want_order = GetWantOrder(&want_count);
    PrintOrder("[user]", user_order, user_count);
    PrintOrder("[want]", want_order, want_count);
}

static int RunList(void) {
    ALGraph G; /* G：采用邻接表存储的实例图。 */
    VertexType
        user_order[MAX_VERTEX_NUM]; /* user_order：用户算法得到的 DFS 次序。 */
    int user_count;                 /* user_count：用户算法访问的顶点个数。 */

    if (!CreateALGraph(&G)) {
        fprintf(stderr, "创建邻接表失败：内存不足。\n");
        return 0;
    }

    puts("[list]");
    DFSTraverseList(&G, user_order, &user_count);
    PrintResult(user_order, user_count);
    DestroyALGraph(&G);
    return 1;
}

static void RunMatrix(void) {
    MGraph G; /* G：采用邻接矩阵存储的实例图。 */
    VertexType
        user_order[MAX_VERTEX_NUM]; /* user_order：用户算法得到的 DFS 次序。 */
    int user_count;                 /* user_count：用户算法访问的顶点个数。 */

    CreateMGraph(&G);
    puts("[matrix]");
    DFSTraverseMatrix(&G, user_order, &user_count);
    PrintResult(user_order, user_count);
}

int main(int argc, char* argv[]) {
    int run_list;   /* run_list：是否运行邻接表 DFS。 */
    int run_matrix; /* run_matrix：是否运行邻接矩阵 DFS。 */

    /* argc：命令行参数个数；argv：命令行参数数组。 */
    run_list = (argc == 1 || (argc == 2 && strcmp(argv[1], "-list") == 0));
    run_matrix = (argc == 1 || (argc == 2 && strcmp(argv[1], "-matrix") == 0));

    if (argc > 2 || (!run_list && !run_matrix)) {
        fprintf(stderr, "用法：%s [-list|-matrix]\n", argv[0]);
        return 1;
    }

    if (run_list && !RunList()) {
        return 1;
    }
    if (run_matrix) {
        RunMatrix();
    }
    return 0;
}
