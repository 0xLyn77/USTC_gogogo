/*
 * 邻接矩阵 BFS 完整版（C++ 学习参考）
 * ------------------------------------------------------------
 * static void BFS(const MGraph& G, int v, std::vector<bool>& visited,
 *                 std::vector<VertexType>& order)
 * {
 *     std::queue<int> Q; // Q：保存待处理顶点下标的队列。
 *     int w;             // w：从矩阵第 v 行找到的候选邻接点下标。
 *
 *     visited[v] = true;
 *     order.push_back(G.vexs[v]);
 *     Q.push(v);
 *
 *     while (!Q.empty()) {
 *         v = Q.front();
 *         Q.pop();
 *
 *         for (w = 0; w < G.vexnum; ++w) {
 *             if (G.arcs[v][w] != 0 && !visited[w]) {
 *                 visited[w] = true;
 *                 order.push_back(G.vexs[w]);
 *                 Q.push(w);
 *             }
 *         }
 *     }
 * }
 *
 * void BFSTraverseMatrix(const MGraph& G, std::vector<VertexType>& order)
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

void BFSTraverseMatrix(const MGraph& G, std::vector<VertexType>& user_order) {
    user_order.clear();
    std::vector<bool> visited(G.vexnum, false);
    std::queue<int> q;

    q.push(0);
    visited[0] = true;
    user_order.push_back(G.vexs[0]);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (int w = 0; w < G.vexnum; w++) {
            if (!visited[w] && G.arcs[v][w] == 1) {
                q.push(w);
                visited[w] = true;
                user_order.push_back(G.vexs[w]);
            }
        }
    }
}
