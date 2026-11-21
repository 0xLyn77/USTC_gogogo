#include <iostream>
#include <string>
#include <vector>

#include "graph.hpp"

static void PrintOrder(const std::string& tag,
                       const std::vector<VertexType>& order) {
    std::size_t i; /* i：遍历访问次序的循环变量。 */

    std::cout << tag << '\t';
    for (i = 0; i < order.size(); ++i) {
        std::cout << ' ' << order[i];
    }
    std::cout << '\n';
}

static void PrintResult(const std::vector<VertexType>& user_order) {
    const std::vector<VertexType>& want_order = GetWantOrder();
    /* want_order：本实例图的标准 BFS 次序。 */

    PrintOrder("[user]", user_order);
    PrintOrder("[want]", want_order);
}

static void RunList() {
    ALGraph G{}; /* G：采用邻接表存储的实例图。 */
    std::vector<VertexType>
        user_order; /* user_order：用户算法得到的 BFS 次序。 */

    CreateALGraph(G);
    std::cout << "[list]\n";
    BFSTraverseList(G, user_order);
    PrintResult(user_order);
    DestroyALGraph(G);
}

static void RunMatrix() {
    MGraph G{}; /* G：采用邻接矩阵存储的实例图。 */
    std::vector<VertexType>
        user_order; /* user_order：用户算法得到的 BFS 次序。 */

    CreateMGraph(G);
    std::cout << "[matrix]\n";
    BFSTraverseMatrix(G, user_order);
    PrintResult(user_order);
}

int main(int argc, char* argv[]) {
    bool run_list;   /* run_list：是否运行邻接表 BFS。 */
    bool run_matrix; /* run_matrix：是否运行邻接矩阵 BFS。 */

    /* argc：命令行参数个数；argv：命令行参数数组。 */
    run_list = (argc == 1 || (argc == 2 && std::string(argv[1]) == "-list"));
    run_matrix =
        (argc == 1 || (argc == 2 && std::string(argv[1]) == "-matrix"));

    if (argc > 2 || (!run_list && !run_matrix)) {
        std::cerr << "用法：" << argv[0] << " [-list|-matrix]\n";
        return 1;
    }

    if (run_list) {
        RunList();
    }
    if (run_matrix) {
        RunMatrix();
    }
    return 0;
}
