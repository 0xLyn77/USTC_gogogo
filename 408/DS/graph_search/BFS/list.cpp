/*
 * 邻接表 BFS 完整版（C++ 学习参考）
 * ------------------------------------------------------------
 * static void BFS(const ALGraph& G, int v, std::vector<bool>& visited,
 *                 std::vector<VertexType>& order)
 * {
 *     std::queue<int> Q; // Q：保存待处理顶点下标的队列。
 *     EdgeNode* p;       // p：指向顶点 v 当前邻接边的指针。
 *
 *     visited[v] = true;
 *     order.push_back(G.vertices[v].data);
 *     Q.push(v);
 *
 *     while (!Q.empty()) {
 *         v = Q.front();
 *         Q.pop();
 *         p = G.vertices[v].firstedge;
 *
 *         while (p != nullptr) {
 *             if (!visited[p->adjvex]) {
 *                 visited[p->adjvex] = true;
 *                 order.push_back(G.vertices[p->adjvex].data);
 *                 Q.push(p->adjvex);
 *             }
 *             p = p->nextedge;
 *         }
 *     }
 * }
 *
 * void BFSTraverseList(const ALGraph& G, std::vector<VertexType>& order)
 * {
 *     std::vector<bool> visited(G.vexnum, false); //
 * visited：顶点访问标记数组。 int v;                                      //
 * v：当前检查的顶点下标。
 *
 *     order.clear();
 *     for (v = 0; v < G.vexnum; ++v) {
 *         if (!visited[v]) {
 *             BFS(G, v, visited, order);
 *         }
 *     }
 * }
 * ------------------------------------------------------------
 * 说明：std::queue 已经封装队头和队尾，不再需要手写 front、rear；
 *       std::vector::push_back 会自动记录结果长度，不再需要 order_count。
 */

#include <queue>
#include <vector>

#include "graph.hpp"

void BFSTraverseList(const ALGraph& G, std::vector<VertexType>& user_order) {
    user_order.clear();
    std::vector<bool> visited(G.vexnum, false);
    std::queue<int> q;

    q.push(0);
    visited[0] = true;
    user_order.push_back(G.vertices[0].data);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        EdgeNode* en = G.vertices[v].firstedge;

        while (en != nullptr) {
            if (!visited[en->adjvex]) {
                q.push(en->adjvex);
                visited[en->adjvex] = true;
                user_order.push_back(G.vertices[en->adjvex].data);
            }
            en = en->nextedge;
        }
    }
}
