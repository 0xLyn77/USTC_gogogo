#include <stdexcept>

#include "graph.hpp"

int FindClosestUnsettled(const std::vector<Weight>* dist,
                         const std::vector<bool>* final) {
    Weight min = INF;
    size_t minIndex = 0;

    for (size_t i = 0; i < dist->size(); i++) {
        if (dist->at(i) < min && !final->at(i)) {
            minIndex = i;
            min = dist->at(i);
        }
    }

    return minIndex;
}

DijkstraResult Dijkstra(const MGraph& G, int source) {
    // init result and final[]
    DijkstraResult result{
        std::vector<Weight>(G.vexnum, INF),  // dist
        std::vector<int>(G.vexnum, -1)       // prev
    };
    std::vector<bool> final(G.vexnum, false);

    result.dist[source] = 0;
    // vex's index to handle
    int present;
    for (int i = 0; i < G.vexnum; i++) {
        // choose the minimalist in the G.dist[] to handle
        present = FindClosestUnsettled(&result.dist, &final);

        final[present] = true;

        // find the vexs connected with present
        for (int j = 0; j < G.vexnum; j++) {
            if (G.arcs[present][j] != INF) {
                // sum the weight between present and vexs with dist[present]
                Weight pre = G.arcs[present][j] + result.dist[present];
                // if sum is smaller than dist[vex], then replace it
                if (pre < result.dist[j]) {
                    result.dist[j] = pre;
                    result.prev[j] = present;
                }
            }
        }
    }

    return result;
}
