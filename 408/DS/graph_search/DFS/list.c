/*
 * 邻接表 DFS 完整版（学习参考）
 * ------------------------------------------------------------
 * static void DFS(const ALGraph *G, int v, int visited[],
 *                 VertexType order[], int *order_count)
 * {
 *     EdgeNode *p;  // p：指向顶点 v 当前邻接边的指针。
 *
 *     visited[v] = 1;
 *     order[*order_count] = G->vertices[v].data;
 *     ++(*order_count);
 *
 *     p = G->vertices[v].firstedge;
 *     while (p != NULL) {
 *         if (!visited[p->adjvex]) {
 *             DFS(G, p->adjvex, visited, order, order_count);
 *         }
 *         p = p->nextedge;
 *     }
 * }
 *
 * void DFSTraverseList(const ALGraph *G, VertexType order[], int *order_count)
 * {
 *     int visited[MAX_VERTEX_NUM] = {0}; // visited：顶点访问标记数组。
 *     int v;                             // v：当前检查的顶点下标。
 *
 *     *order_count = 0;
 *     for (v = 0; v < G->vexnum; ++v) {
 *         if (!visited[v]) {
 *             DFS(G, v, visited, order, order_count);
 *         }
 *     }
 * }
 * ------------------------------------------------------------
 * 说明：上面的外层循环可以处理非连通图；递归 DFS 负责访问一个连通分量。
 */

#include "graph.h"
#include <stdio.h>

static void DFS(const ALGraph* G, int v, int visited[], VertexType user_order[],
                int* order_count) {
    visited[v] = 1;
    user_order[*order_count] = G->vertices[v].data;
    ++(*order_count);

    EdgeNode* en = G->vertices[v].firstedge;
    while (en != NULL) {
        if (visited[en->adjvex] != 1) {
            DFS(G, en->adjvex, visited, user_order, order_count);
        }
        en = en->nextedge;
    }
}

void DFSTraverseList(const ALGraph* G, VertexType user_order[],
                     int* order_count) {
    *order_count = 0;
    int visited[MAX_VERTEX_NUM] = {0};
    int v;

    for (v = 0; v < G->vexnum; v++) {
        if (visited[v] != 1) {
            DFS(G, v, visited, user_order, order_count);
        }
    }
}
