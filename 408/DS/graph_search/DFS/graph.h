#ifndef GRAPH_H
#define GRAPH_H

#define MAX_VERTEX_NUM 20

typedef char VertexType;

typedef struct EdgeNode {
    int adjvex;                /* 该边所指向的顶点在顶点表中的下标 */
    struct EdgeNode* nextedge; /* 指向下一条邻接边的指针 */
} EdgeNode;

typedef struct VNode {
    VertexType data;     /* 顶点中保存的数据 */
    EdgeNode* firstedge; /* 指向该顶点第一条邻接边的指针 */
} VNode;

typedef struct {
    VNode vertices[MAX_VERTEX_NUM]; /* 邻接表中的顶点表 */
    int vexnum;                     /* 图的当前顶点数 */
    int arcnum;                     /* 图的当前边数 */
} ALGraph;

typedef struct {
    VertexType vexs[MAX_VERTEX_NUM];          /* 邻接矩阵中的顶点表 */
    int arcs[MAX_VERTEX_NUM][MAX_VERTEX_NUM]; /* 图的邻接矩阵 */
    int vexnum;                               /* 图的当前顶点数 */
    int arcnum;                               /* 图的当前边数 */
} MGraph;

/* model.c：创建、销毁实例图，并提供本实例的标准 DFS 次序。 */
int CreateALGraph(ALGraph* G);
void DestroyALGraph(ALGraph* G);
void CreateMGraph(MGraph* G);
const VertexType* GetWantOrder(int* order_count);

/* list.c：基于邻接表的 DFS（核心函数已留空）。 */
void DFSTraverseList(const ALGraph* G, VertexType user_order[],
                     int* order_count);

/* matrix.c：基于邻接矩阵的 DFS（核心函数已留空）。 */
void DFSTraverseMatrix(const MGraph* G, VertexType user_order[],
                       int* order_count);

#endif
