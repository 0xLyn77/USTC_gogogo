#ifndef SHORTEST_PATH_GRAPH_HPP
#define SHORTEST_PATH_GRAPH_HPP

#include <limits>
#include <string>
#include <vector>

constexpr int MAX_VERTEX_NUM = 20;
using VertexType = char;
using Weight = long long;
constexpr Weight INF = std::numeric_limits<Weight>::max() / 4;

struct MGraph {
    VertexType vexs[MAX_VERTEX_NUM]{};
    Weight arcs[MAX_VERTEX_NUM][MAX_VERTEX_NUM]{};
    int vexnum = 0;
    int arcnum = 0; // 有向弧数量；无向边存成两个方向。
};

struct DijkstraResult {
    std::vector<Weight> dist;
    std::vector<int> prev; // 前驱下标；源点和不可达顶点为 -1。
};

struct FloydResult {
    std::vector<std::vector<Weight>> dist;
    std::vector<std::vector<int>> next; // i 到 j 路径上，i 后面的第一个顶点。
    // next[i][i] = i；不可达时 next[i][j] = -1。
};

struct TestCase {
    std::string name;
    std::string purpose;
    MGraph graph;
    bool dijkstra_allowed = true;
};

// 你只需要实现下面两个函数。输入图不应修改。
DijkstraResult Dijkstra(const MGraph& G, int source);
FloydResult Floyd(const MGraph& G);

std::vector<TestCase> CreateTestCases();
bool VerifyDijkstra(const TestCase& test, int source,
                    const DijkstraResult& result, std::string& error);
bool VerifyFloyd(const TestCase& test, const FloydResult& result,
                 std::string& error);
void PrintGraph(const TestCase& test);

#endif
