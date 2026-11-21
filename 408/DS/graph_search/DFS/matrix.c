#include "graph.h"

/*
 * FOR EXAMPLE
 *
 * [matrix]
 *     0 1 2 3
 * 0   0 1 1 0
 * 1   1 0 0 0
 * 2   1 0 0 1
 * 3   0 0 1 0
 *
 * [DFSTraverseMatrix]
 *     |
 *     | v = 0，visited[0] == 0
 *     v
 * [DFS(0)] 扫描 matrix[0][0...3]
 *     |
 *     | matrix[0][1] == 1
 *     v
 * [DFS(1)] 扫描 matrix[1][0...3]
 *     |
 *     | 顶点 0 已访问，没有其他未访问邻接点
 *     v
 * return DFS(0)，继续扫描 matrix[0][2]
 *     |
 *     | matrix[0][2] == 1
 *     v
 * [DFS(2)] 扫描 matrix[2][0...3]
 *     |
 *     | matrix[2][3] == 1
 *     v
 * [DFS(3)] 扫描 matrix[3][0...3]
 *     |
 *     | 顶点 2 已访问
 *     v
 * return DFS(2) -> return DFS(0) -> return DFSTraverseMatrix
 *
 * DFS order: 0 -> 1 -> 2 -> 3
 */

/*
 * DFS function 负责遍历每一个顶点的边
 */
static void DFS(const MGraph* G, int v, int visited[], VertexType user_order[],
                int* order_count) {
    int w;
    if (v < G->vexnum) {
        visited[v] = 1;
        user_order[*order_count] = G->vexs[v];
        ++(*order_count);

        for (w = 0; w < G->vexnum; w++) {
            if (visited[w] != 1 && G->arcs[v][w] != 0) {
                DFS(G, w, visited, user_order, order_count);
            }
        }
    }
}

/*
 * 该function管理顶点
 * 每个顶点传入DFS function
 * order_count -> index & user_order -> array 负责构建遍历array
 */
void DFSTraverseMatrix(const MGraph* G, VertexType user_order[],
                       int* order_count) {
    *order_count = 0;
    int visited[MAX_VERTEX_NUM] = {0};
    int v;

    for (v = 0; v < G->vexnum; v++) {
        if (!visited[v]) {
            DFS(G, v, visited, user_order, order_count);
        }
    }
}
