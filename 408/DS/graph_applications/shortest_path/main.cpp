#include <exception>
#include <iostream>
#include <string>

#include "graph.hpp"

int main(int argc, char* argv[]) {
    std::string mode = "all";
    std::string case_name;
    bool show = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-dijkstra") mode = "dijkstra";
        else if (arg == "-floyd") mode = "floyd";
        else if (arg == "--show") show = true;
        else if (arg == "--case" && i + 1 < argc) case_name = argv[++i];
        else {
            std::cerr << "用法：" << argv[0] << " [-dijkstra|-floyd] [--case 名称] [--show]\n";
            return 2;
        }
    }
    int passed = 0;
    int failed = 0;
    int skipped = 0;
    bool found = false;
    for (const auto& test : CreateTestCases()) {
        if (!case_name.empty() && case_name != test.name) continue;
        found = true;
        if (show) { PrintGraph(test); continue; }
        if (mode != "floyd") {
            if (!test.dijkstra_allowed) {
                ++skipped;
            } else {
                // 每张图的所有顶点都作为源点，避免硬编码从 A 出发。
                bool ok = true;
                std::string error;
                for (int s = 0; s < test.graph.vexnum; ++s) {
                    try {
                        ok = VerifyDijkstra(test, s, Dijkstra(test.graph, s), error);
                    } catch (const std::exception& e) {
                        ok = false;
                        error = e.what();
                    }
                    if (!ok) {
                        error = "源点 " + std::string(1, test.graph.vexs[s]) + "：" + error;
                        break;
                    }
                }
                std::cout << (ok ? "[PASS] " : "[FAIL] ") << "Dijkstra / " << test.name;
                if (!ok) std::cout << " / " << test.purpose << " / " << error;
                std::cout << '\n';
                ok ? ++passed : ++failed;
            }
        }
        if (mode != "dijkstra") {
            bool ok = false;
            std::string error;
            try {
                ok = VerifyFloyd(test, Floyd(test.graph), error);
            } catch (const std::exception& e) {
                error = e.what();
            }
            std::cout << (ok ? "[PASS] " : "[FAIL] ") << "Floyd / " << test.name;
            if (!ok) std::cout << " / " << test.purpose << " / " << error;
            std::cout << '\n';
            ok ? ++passed : ++failed;
        }
    }
    if (!found) {
        std::cerr << "没有找到用例：" << case_name << '\n';
        return 2;
    }
    if (show) return 0;
    std::cout << "\n通过 " << passed << "，失败 " << failed
              << "，Dijkstra 跳过负权用例 " << skipped << '\n';
    if (passed == 0 && failed == 0) {
        std::cerr << "未执行有效验证，请选择非负用例或运行 Floyd。\n";
        return 2;
    }
    return failed == 0 ? 0 : 1;
}
