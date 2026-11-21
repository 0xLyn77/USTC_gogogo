#include <iostream>

#include "graph.hpp"

namespace {
bool Fail(std::string& error, const std::string& message) {
    error = message;
    return false;
}
} // namespace

bool VerifyResult(const TestCase& test,
                  const std::vector<int>& order, std::string& error) {
    const MGraph& G = test.graph;
    if (order.size() != static_cast<std::size_t>(G.vexnum)) {
        return Fail(error, "应输出 " + std::to_string(G.vexnum) + " 个顶点，实际为 " + std::to_string(order.size()));
    }
    std::vector<int> position(G.vexnum, -1);
    for (int i = 0; i < G.vexnum; ++i) {
        const int v = order[i];
        if (v < 0 || v >= G.vexnum) return Fail(error, "顶点下标越界：" + std::to_string(v));
        if (position[v] != -1) return Fail(error, "顶点重复：" + std::string(1, G.vexs[v]));
        position[v] = i;
    }
    for (int u = 0; u < G.vexnum; ++u) {
        for (int v = 0; v < G.vexnum; ++v) {
            if (G.arcs[u][v] && position[u] >= position[v]) {
                return Fail(error, "依赖 " + std::string(1, G.vexs[u]) + " → " + G.vexs[v] +
                            " 不满足：" + G.vexs[u] + " 必须排在 " + G.vexs[v] + " 前面");
            }
        }
    }
    return true;
}

void PrintOrder(const MGraph& G, const std::vector<int>& order) {
    std::cout << "[user]";
    for (int v : order) {
        if (v >= 0 && v < G.vexnum) std::cout << ' ' << G.vexs[v];
        else std::cout << " [非法下标:" << v << ']';
    }
    std::cout << '\n';
}

void PrintGraph(const TestCase& test) {
    const MGraph& G = test.graph;
    std::cout << test.name << "：" << test.purpose << '\n' << "顶点：";
    for (int i = 0; i < G.vexnum; ++i) std::cout << ' ' << G.vexs[i] << '=' << i;
    std::cout << "\n依赖（起点活动必须先于终点活动）：\n";
    for (int i = 0; i < G.vexnum; ++i) {
        for (int j = 0; j < G.vexnum; ++j) {
            if (G.arcs[i][j]) std::cout << "  " << G.vexs[i] << " → " << G.vexs[j] << '\n';
        }
    }
}
