# AOV 网

通过弧来表示事情先后关系的有向图

## 拓扑排序

选择一个无前驱的节点 访问并删除他 then repeat

## 拓扑排序验证模板

### 运行

```sh
cd /Users/lyn-77/CODE/learn/408/data_structures/aov_net
make run                           # 验证全部无环图
make run-dag                       # 与 make run 相同，保留命令别名
make run ARGS="--case lesson"      # 只测课堂图
make show ARGS="--case lesson"     # 查看课堂图数据
```

### 数据与验证

8 张专项 DAG、30 张固定种子随机 DAG，全部无环：

| 用例 | 检查点 |
| --- | --- |
| single / independent | 单活动、没有依赖时仍需输出全部活动 |
| lesson | A→B→E，C→E，D→E；E 必须等 B、C、D 都完成 |
| chain / reverse_chain | 顺序唯一，下标不一定按依赖顺序递增 |
| diamond | 分叉后汇合，只有全部前驱都处理完才能输出汇合点 |
| disconnected | 独立活动和多个分量，不能只遍历一个起点 |
| dense | 一个活动有多个前驱，检查所有依赖 |
| random_0 .. random_29 | 随机拓扑结构、打乱下标、最多 20 个顶点 |

合法拓扑序通常不唯一。例如 lesson 的 `A B C D E` 和 `D C A B E` 都会通过。
验证器不比较固定排列，而是检查输出长度、重复/越界下标，以及每条弧的先后关系。
同一图会重复调用函数，验证旧输出是否清空、局部状态是否重新初始化。

`graph.hpp` 定义接口，`model.cpp` 生成图，`verify.cpp` 判断合法性，`main.cpp` 运行并报告错误，`topological.cpp` 留给你写算法。
