#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>

constexpr int MAX_VERTEX_NUM = 20;

using VertexType = char;

struct EdgeNode {
    int adjvex;         /* 该边所指向的顶点在顶点表中的下标。 */
    EdgeNode* nextedge; /* 指向下一条邻接边的指针。 */
};

struct VNode {
    VertexType data;     /* 顶点中保存的数据。 */
    EdgeNode* firstedge; /* 指向该顶点第一条邻接边的指针。 */
};

struct ALGraph {
    VNode vertices[MAX_VERTEX_NUM]; /* 邻接表中的顶点表。 */
    int vexnum;                     /* 图的当前顶点数。 */
    int arcnum;                     /* 图的当前边数。 */
};

struct MGraph {
    VertexType vexs[MAX_VERTEX_NUM];          /* 邻接矩阵中的顶点表。 */
    int arcs[MAX_VERTEX_NUM][MAX_VERTEX_NUM]; /* 图的邻接矩阵。 */
    int vexnum;                               /* 图的当前顶点数。 */
    int arcnum;                               /* 图的当前边数。 */
};

/* model.cpp：创建、销毁实例图，并提供本实例的标准 BFS 次序。 */
void CreateALGraph(ALGraph& G);
void DestroyALGraph(ALGraph& G);
void CreateMGraph(MGraph& G);
const std::vector<VertexType>& GetWantOrder();

/* list.cpp：基于邻接表的 BFS（核心函数已留空）。 */
void BFSTraverseList(const ALGraph& G, std::vector<VertexType>& user_order);

/* matrix.cpp：基于邻接矩阵的 BFS（核心函数已留空）。 */
void BFSTraverseMatrix(const MGraph& G, std::vector<VertexType>& user_order);

#endif
