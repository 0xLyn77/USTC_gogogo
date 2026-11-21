#include <exception>
#include <iostream>
#include <string>

#include "graph.hpp"

int main(int argc, char* argv[]) {
    std::string case_name;
    bool show = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--case" && i + 1 < argc) case_name = argv[++i];
        else if (arg == "--show") show = true;
        else {
            std::cerr << "用法：" << argv[0] << " [--case 名称] [--show]\n";
            return 2;
        }
    }

    int passed = 0;
    int failed = 0;
    bool found = false;
    for (const auto& test : CreateTestCases()) {
        if (!case_name.empty() && test.name != case_name) continue;
        found = true;
        if (show) { PrintGraph(test); continue; }
        // 预放一个值，检查算法是否清空旧输出。真实顶点下标均为非负数。
        std::vector<int> order{-99};
        std::string error;
        bool ok = false;
        try {
            TopologicalSort(test.graph, order);
            ok = VerifyResult(test, order, error);
            // 同一图再调用一次，检查残留状态和忘记清空 order。
            if (ok) {
                TopologicalSort(test.graph, order);
                ok = VerifyResult(test, order, error);
                if (!ok) error = "重复调用失败：" + error;
            }
        } catch (const std::exception& e) {
            ok = false;
            error = e.what();
        }
        std::cout << (ok ? "[PASS] " : "[FAIL] ") << test.name;
        if (!ok) {
            std::cout << " / " << test.purpose << " / " << error << '\n';
            PrintOrder(test.graph, order);
        } else std::cout << '\n';
        ok ? ++passed : ++failed;
    }
    if (!found) {
        std::cerr << "没有匹配的用例，请检查 --case。\n";
        return 2;
    }
    if (show) return 0;
    std::cout << "\n通过 " << passed << "，失败 " << failed << '\n';
    return failed == 0 ? 0 : 1;
}
