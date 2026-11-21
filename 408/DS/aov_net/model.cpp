#include <algorithm>
#include <initializer_list>
#include <numeric>
#include <random>

#include "graph.hpp"

namespace {
struct Edge { int from; int to; };

MGraph EmptyGraph(int n) {
    MGraph G{};
    G.vexnum = n;
    for (int i = 0; i < n; ++i) G.vexs[i] = static_cast<char>('A' + i);
    return G;
}

void AddEdge(MGraph& G, int from, int to) {
    if (G.arcs[from][to] == 0) {
        G.arcs[from][to] = 1;
        ++G.arcnum;
    }
}

MGraph Build(int n, std::initializer_list<Edge> edges) {
    MGraph G = EmptyGraph(n);
    for (const auto& edge : edges) AddEdge(G, edge.from, edge.to);
    return G;
}
} // namespace

std::vector<TestCase> CreateTestCases() {
    std::vector<TestCase> cases = {
        {"single", "单活动，必须输出它", Build(1, {})},
        {"independent", "没有依赖，任意顶点排列都合法", Build(5, {})},
        {"lesson", "课堂图：A→B→E，C→E，D→E", Build(5, {{0,1}, {1,4}, {2,4}, {3,4}})},
        {"chain", "单链，顺序唯一", Build(5, {{0,1}, {1,2}, {2,3}, {3,4}})},
        {"reverse_chain", "反向下标的单链，不能直接输出 A B C D E",
         Build(5, {{4,3}, {3,2}, {2,1}, {1,0}})},
        {"diamond", "分叉再汇合：汇合点需等待全部前驱",
         Build(4, {{0,1}, {0,2}, {1,3}, {2,3}})},
        {"disconnected", "两条独立链和一个独立活动，不能只从 A 遍历",
         Build(6, {{0,1}, {1,2}, {3,4}})},
        {"dense", "大量前驱：每条依赖都必须满足",
         Build(5, {{0,1}, {0,2}, {0,3}, {0,4}, {1,2}, {1,3}, {1,4},
                   {2,3}, {2,4}, {3,4}})},

    };

    std::mt19937 rng(4082026);
    for (int id = 0; id < 30; ++id) {
        const int n = id == 29 ? MAX_VERTEX_NUM : 2 + static_cast<int>(rng() % 9);
        MGraph G = EmptyGraph(n);
        // 先打乱一个合法次序，只沿这个次序向后加弧，保证生成 DAG。
        std::vector<int> permutation(n);
        std::iota(permutation.begin(), permutation.end(), 0);
        for (int i = n - 1; i > 0; --i) {
            std::swap(permutation[i], permutation[rng() % (i + 1)]);
        }
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (rng() % 100 < 35) AddEdge(G, permutation[i], permutation[j]);
            }
        }
        cases.push_back({"random_" + std::to_string(id), "固定种子 DAG，下标不代表先后关系", G});
    }
    return cases;
}
