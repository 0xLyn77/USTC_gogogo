#include <algorithm>
#include <iostream>
#include <sstream>

#include "graph.hpp"

namespace {
// 独立判定器：用 Bellman-Ford 生成标准距离，不调用待实现的两个函数。
std::vector<Weight> Expected(const MGraph& G, int source) {
    std::vector<Weight> dist(G.vexnum, INF);
    dist[source] = 0;
    for (int pass = 1; pass < G.vexnum; ++pass) {
        bool changed = false;
        for (int u = 0; u < G.vexnum; ++u) {
            for (int v = 0; v < G.vexnum; ++v) {
                if (dist[u] == INF || G.arcs[u][v] == INF) continue;
                const Weight candidate = dist[u] + G.arcs[u][v];
                if (candidate < dist[v]) {
                    dist[v] = candidate;
                    changed = true;
                }
            }
        }
        if (!changed) break;
    }
    return dist;
}

std::string Value(Weight w) { return w == INF ? "INF" : std::to_string(w); }

bool Fail(std::string& error, const std::string& message) {
    error = message;
    return false;
}

std::string Pair(const MGraph& G, int u, int v) {
    return std::string(1, G.vexs[u]) + " -> " + G.vexs[v];
}

bool CheckDistance(const MGraph& G, int source, int target, Weight actual,
                   Weight expected, std::string& error) {
    if (actual == expected) return true;
    return Fail(error, Pair(G, source, target) + " 距离错误：[user] " +
                Value(actual) + "，[want] " + Value(expected));
}

bool CheckPath(const MGraph& G, const std::vector<int>& path, Weight expected,
               std::string& error) {
    Weight sum = 0;
    for (std::size_t i = 1; i < path.size(); ++i) {
        const Weight w = G.arcs[path[i-1]][path[i]];
        if (w == INF) return Fail(error, "路径经过不存在的边");
        sum += w;
    }
    if (sum != expected) {
        return Fail(error, "路径权重为 " + Value(sum) + "，最短距离为 " + Value(expected));
    }
    return true;
}
} // namespace

bool VerifyDijkstra(const TestCase& test, int source,
                    const DijkstraResult& result, std::string& error) {
    const MGraph& G = test.graph;
    const int n = G.vexnum;
    if (result.dist.size() != static_cast<std::size_t>(n) ||
        result.prev.size() != static_cast<std::size_t>(n)) {
        return Fail(error, "dist 和 prev 的长度必须都为 " + std::to_string(n));
    }
    const auto want = Expected(G, source);
    for (int target = 0; target < n; ++target) {
        if (!CheckDistance(G, source, target, result.dist[target], want[target], error)) return false;
        if (target == source || want[target] == INF) {
            if (result.prev[target] != -1) return Fail(error, "源点及不可达顶点的 prev 必须为 -1");
            continue;
        }
        std::vector<int> path;
        std::vector<bool> seen(n, false);
        int current = target;
        while (current != source) {
            if (current < 0 || current >= n) return Fail(error, "前驱下标越界或路径未到达源点");
            if (seen[current]) return Fail(error, "前驱链中出现环");
            seen[current] = true;
            path.push_back(current);
            current = result.prev[current];
        }
        path.push_back(source);
        std::reverse(path.begin(), path.end());
        if (!CheckPath(G, path, want[target], error)) {
            error = Pair(G, source, target) + "：" + error;
            return false;
        }
    }
    return true;
}

bool VerifyFloyd(const TestCase& test, const FloydResult& result,
                 std::string& error) {
    const MGraph& G = test.graph;
    const int n = G.vexnum;
    if (result.dist.size() != static_cast<std::size_t>(n) ||
        result.next.size() != static_cast<std::size_t>(n)) {
        return Fail(error, "dist 和 next 必须有 " + std::to_string(n) + " 行");
    }
    for (int i = 0; i < n; ++i) {
        if (result.dist[i].size() != static_cast<std::size_t>(n) ||
            result.next[i].size() != static_cast<std::size_t>(n)) {
            return Fail(error, "dist 和 next 每行必须有 " + std::to_string(n) + " 列");
        }
    }
    for (int source = 0; source < n; ++source) {
        const auto want = Expected(G, source);
        for (int target = 0; target < n; ++target) {
            if (!CheckDistance(G, source, target, result.dist[source][target], want[target], error)) return false;
            if (source == target || want[target] == INF) {
                const int next = source == target ? source : -1;
                if (result.next[source][target] != next) return Fail(error, "自身 next 必须等于自身下标；不可达 next 必须为 -1");
                continue;
            }
            std::vector<int> path{source};
            std::vector<bool> seen(n, false);
            int current = source;
            while (current != target) {
                if (seen[current]) return Fail(error, Pair(G, source, target) + " 的 next 路径出现环");
                seen[current] = true;
                current = result.next[current][target];
                if (current < 0 || current >= n) return Fail(error, "next 下标越界或路径无法到达终点");
                path.push_back(current);
            }
            if (!CheckPath(G, path, want[target], error)) {
                error = Pair(G, source, target) + "：" + error;
                return false;
            }
        }
    }
    return true;
}

void PrintGraph(const TestCase& test) {
    const MGraph& G = test.graph;
    std::cout << test.name << "：" << test.purpose << '\n';
    std::cout << "顶点下标：";
    for (int i = 0; i < G.vexnum; ++i) std::cout << ' ' << G.vexs[i] << '=' << i;
    std::cout << "\n有向边（未列出即无边，矩阵对角线为 0）：\n";
    for (int i = 0; i < G.vexnum; ++i) {
        for (int j = 0; j < G.vexnum; ++j) {
            if (i != j && G.arcs[i][j] != INF) {
                std::cout << "  " << Pair(G, i, j) << " : " << G.arcs[i][j] << '\n';
            }
        }
    }
}
