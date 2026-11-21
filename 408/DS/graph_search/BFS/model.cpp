/*
 * 实例图（无向图）
 *
 * 顶点：A、B、C、D、E、F
 * 边：  A-B、A-C、B-D、B-E、C-F、E-F
 *
 * 按顶点下标从小到大访问邻接点，从 A 开始的标准 BFS 次序为：
 * A B C D E F
 */

#include <vector>

#include "graph.hpp"

static constexpr VertexType vertex_data[] = {'A', 'B', 'C', 'D', 'E', 'F'};
/* vertex_data：实例图的顶点数据，数组下标就是顶点编号。 */

static constexpr int edge_data[][2] = {{0, 1}, {0, 2}, {1, 3},
                                       {1, 4}, {2, 5}, {4, 5}};
/* edge_data：实例图的无向边集合，每行保存一条边的两个顶点下标。 */

static const std::vector<VertexType> want_order = {'A', 'B', 'C',
                                                   'D', 'E', 'F'};
/* want_order：按顶点下标递增访问邻接点时的标准 BFS 次序。 */

/* G：待写入的邻接表；from：边的起点；to：边的终点。 */
static void AppendEdgeNode(ALGraph& G, int from, int to) {
    EdgeNode* new_edge; /* new_edge：当前新建的边结点。 */
    EdgeNode** link;    /* link：指向待写入链指针的位置。 */

    new_edge = new EdgeNode{to, nullptr};
    link = &G.vertices[from].firstedge;
    while (*link != nullptr) {
        link = &(*link)->nextedge;
    }
    *link = new_edge;
}

void CreateALGraph(ALGraph& G) {
    int i;          /* i：遍历顶点或边数组的循环变量。 */
    int edge_count; /* edge_count：实例图中的无向边数。 */
    int from;       /* from：当前无向边的起点下标。 */
    int to;         /* to：当前无向边的终点下标。 */

    G.vexnum = static_cast<int>(sizeof(vertex_data) / sizeof(vertex_data[0]));
    G.arcnum = 0;

    for (i = 0; i < G.vexnum; ++i) {
        G.vertices[i].data = vertex_data[i];
        G.vertices[i].firstedge = nullptr;
    }

    edge_count = static_cast<int>(sizeof(edge_data) / sizeof(edge_data[0]));
    for (i = 0; i < edge_count; ++i) {
        from = edge_data[i][0];
        to = edge_data[i][1];
        AppendEdgeNode(G, from, to);
        AppendEdgeNode(G, to, from);
        ++G.arcnum;
    }
}

void DestroyALGraph(ALGraph& G) {
    int i;          /* i：遍历顶点表的循环变量。 */
    EdgeNode* p;    /* p：当前准备释放的边结点。 */
    EdgeNode* next; /* next：释放当前结点后要访问的下一结点。 */

    for (i = 0; i < G.vexnum; ++i) {
        p = G.vertices[i].firstedge;
        while (p != nullptr) {
            next = p->nextedge;
            delete p;
            p = next;
        }
        G.vertices[i].firstedge = nullptr;
    }
    G.arcnum = 0;
}

void CreateMGraph(MGraph& G) {
    int i;          /* i：遍历顶点、行或边数组的循环变量。 */
    int j;          /* j：遍历邻接矩阵列的循环变量。 */
    int edge_count; /* edge_count：实例图中的无向边数。 */
    int from;       /* from：当前无向边的起点下标。 */
    int to;         /* to：当前无向边的终点下标。 */

    G.vexnum = static_cast<int>(sizeof(vertex_data) / sizeof(vertex_data[0]));
    G.arcnum = static_cast<int>(sizeof(edge_data) / sizeof(edge_data[0]));

    for (i = 0; i < G.vexnum; ++i) {
        G.vexs[i] = vertex_data[i];
        for (j = 0; j < G.vexnum; ++j) {
            G.arcs[i][j] = 0;
        }
    }

    edge_count = G.arcnum;
    for (i = 0; i < edge_count; ++i) {
        from = edge_data[i][0];
        to = edge_data[i][1];
        G.arcs[from][to] = 1;
        G.arcs[to][from] = 1;
    }
}

const std::vector<VertexType>& GetWantOrder() { return want_order; }
