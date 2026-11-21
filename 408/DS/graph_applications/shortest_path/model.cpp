#include <initializer_list>
#include <random>

#include "graph.hpp"

namespace {
struct Edge { int from; int to; Weight weight; };

MGraph EmptyGraph(int n) {
    MGraph G{};
    G.vexnum = n;
    for (int i = 0; i < n; ++i) {
        G.vexs[i] = static_cast<char>('A' + i);
        for (int j = 0; j < n; ++j) {
            G.arcs[i][j] = i == j ? 0 : INF;
        }
    }
    return G;
}

void AddEdge(MGraph& G, int u, int v, Weight weight) {
    if (G.arcs[u][v] == INF) ++G.arcnum;
    if (weight < G.arcs[u][v]) G.arcs[u][v] = weight;
}

MGraph Build(int n, std::initializer_list<Edge> edges, bool undirected = false) {
    MGraph G = EmptyGraph(n);
    for (const auto& edge : edges) {
        AddEdge(G, edge.from, edge.to, edge.weight);
        if (undirected) AddEdge(G, edge.to, edge.from, edge.weight);
    }
    return G;
}
} // namespace

std::vector<TestCase> CreateTestCases() {
    std::vector<TestCase> cases = {
        {"single", "单顶点：自身距离为 0", Build(1, {})},
        {"classic", "无向图：多轮松弛、从每个顶点出发",
         Build(6, {{0,1,7}, {0,2,9}, {0,5,14}, {1,2,10}, {1,3,15},
                   {2,3,11}, {2,5,2}, {3,4,6}, {4,5,9}}, true)},
        {"relax", "有向图：直接边不一定最短；下标顺序不等于确定顺序",
         Build(5, {{0,1,10}, {0,2,1}, {2,1,1}, {1,3,2}, {2,3,20},
                   {3,4,3}, {0,4,100}, {4,0,100}})},
        {"zero", "零权边、零权环：不能把 0 当作无边",
         Build(5, {{0,1,0}, {1,2,0}, {2,1,0}, {2,3,2}, {0,3,9}, {3,4,0}, {4,0,3}})},
        {"ties", "多条等长最短路：接受任意合法路径",
         Build(5, {{0,1,2}, {0,2,2}, {1,3,3}, {2,3,3}, {1,2,0},
                   {2,1,0}, {3,4,1}, {0,4,6}, {4,0,7}})},
        {"reverse_chain", "Floyd 中间点顺序错误；Dijkstra 固定按下标访问",
         Build(5, {{0,4,1}, {4,3,1}, {3,2,1}, {2,1,1}, {0,1,50}, {1,0,2}})},
        {"large", "长路径超过 32 位 int；INF 不能用很小的常数",
         Build(5, {{0,1,1500000000LL}, {1,2,1500000000LL},
                   {2,3,1500000000LL}, {0,3,6000000000LL}, {3,4,1}, {4,0,1}})},
        {"negative", "仅 Floyd：负权边、负距离，但无负权环",
         Build(5, {{0,1,4}, {0,2,5}, {1,2,-2}, {2,3,3}, {1,3,8},
                   {3,4,2}, {4,0,10}}), false},
        {"negative_cycle_free", "仅 Floyd：含环的负权图，但无负权环",
         Build(6, {{0,1,3}, {1,2,-4}, {2,0,2}, {2,3,2}, {3,4,5},
                   {4,5,-7}, {5,0,10}}), false}
    };

    // 固定 seed，可复现；用生成器输出取模，避免分布实现在平台间不同。
    std::mt19937 rng(4082026);
    for (int id = 0; id < 40; ++id) {
        const int n = id == 39 ? MAX_VERTEX_NUM : 2 + static_cast<int>(rng() % 8);
        MGraph G = EmptyGraph(n);
        // 先构造覆盖全部顶点的有向环，保证任意两点之间都有路径。
        for (int i = 0; i < n; ++i) {
            AddEdge(G, i, (i + 1) % n, 1 + static_cast<Weight>(rng() % 20));
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i != j && rng() % 100 < 35) {
                    AddEdge(G, i, j, static_cast<Weight>(rng() % 21));
                }
            }
        }
        cases.push_back({"random_" + std::to_string(id),
                         "固定种子非负有向图，任意两点可达", G, true});
    }
    return cases;
}
