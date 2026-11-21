#ifndef AOV_GRAPH_HPP
#define AOV_GRAPH_HPP

#include <string>
#include <vector>

constexpr int MAX_VERTEX_NUM = 20;
using VertexType = char;

struct MGraph {
    VertexType vexs[MAX_VERTEX_NUM]{};
    int arcs[MAX_VERTEX_NUM][MAX_VERTEX_NUM]{};
    int vexnum = 0;
    int arcnum = 0;
    // arcs[i][j] == 1：活动 i 必须在活动 j 前完成；0：没有直接依赖。
    // 本练习不带权，也不使用 INF。
};

struct TestCase {
    std::string name;
    std::string purpose;
    MGraph graph;
};

// 唯一需要你实现的函数：输入图只读。
// 输入保证无环；order 存每个顶点的下标，恰好各一次，顺序满足所有弧。
// 顶点没有前驱/后继、图分成多个分量都是合法输入。
void TopologicalSort(const MGraph& G, std::vector<int>& order);

std::vector<TestCase> CreateTestCases();
bool VerifyResult(const TestCase& test,
                  const std::vector<int>& order, std::string& error);
void PrintGraph(const TestCase& test);
void PrintOrder(const MGraph& G, const std::vector<int>& order);

#endif
