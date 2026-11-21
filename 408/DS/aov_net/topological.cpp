#include <stdexcept>

#include "graph.hpp"

void DFS(const MGraph& G, int currentIndex, std::vector<bool>* visited,
         std::vector<int>& order);

void TopologicalSort(const MGraph& G, std::vector<int>& order) {
    // 输出存顶点下标，例如 {0, 2, 1}，而不是字符 {'A', 'C', 'B'}。
    // init
    order.clear();
    std::vector<bool> visited(G.vexnum, false);

    for (int i = 0; i < G.vexnum; i++)
        if (!visited[i]) DFS(G, i, &visited, order);

    std::reverse(order.begin(), order.end());
}

void DFS(const MGraph& G, int currentIndex, std::vector<bool>* visited,
         std::vector<int>& order) {
    visited->at(currentIndex) = true;

    for (int i = 0; i < G.vexnum; i++) {
        if (G.arcs[currentIndex][i] != 0 && !visited->at(i)) {
            DFS(G, i, visited, order);
        }
    }

    order.push_back(currentIndex);
}
