#include <stdexcept>

#include "graph.hpp"

FloydResult Floyd(const MGraph& G) {
    FloydResult result{
        // dist[a][b] for the minimalist weight between a and b
        std::vector<std::vector<Weight>>(
            G.vexnum, std::vector<Weight>(G.vexnum)),  // dist
        // next[a][b] for the next vex where between a and b
        std::vector<std::vector<int>>(G.vexnum,
                                      std::vector<int>(G.vexnum, -1))  // next
    };

    // init the result
    for (int i = 0; i < G.vexnum; i++) {
        for (int j = 0; j < G.vexnum; j++) {
            if (i == j) {
                result.dist[i][j] = 0;
            } else {
                result.dist[i][j] = G.arcs[i][j];
            }
            if (G.arcs[i][j] != INF) {
                /* code */
                result.next[i][j] = j;
            } else {
                result.next[i][j] = -1;
            }
        }
    }

    // for loop the diagonal

    for (int i = 0; i < G.vexnum; i++) {
        // check the vexs in the arcs
        // except the diagonal and the vexs
        // that on the cross line of the selected vex
        Weight pre;
        for (int j = 0; j < G.vexnum; j++) {
            for (int k = 0; k < G.vexnum; k++) {
                if (j != i && k != i && j != k) {
                    // sum the vertical and horizontal weight on the crossline
                    // if sum smaller then change it on the dist
                    // and change the index at the next
                    pre = result.dist[i][k] + result.dist[j][i];
                    if (pre < result.dist[j][k]) {
                        result.dist[j][k] = pre;
                        result.next[j][k] = result.next[j][i];
                    }
                }
            }
        }
    }

    return result;
}
