# 最短路径掌握验证

## 开始练习

```sh
cd /Users/lyn-77/CODE/learn/408/data_structures/graph_applications/shortest_path
make run-dijkstra
make run-floyd
make run
```

只调试一张图或查看完整输入：

```sh
make run-dijkstra ARGS="--case relax"
make run-floyd ARGS="--case negative"
make show ARGS="--case classic"
```

退出码：0 = 全部选中检查通过；1 = 算法未实现或验证失败；2 = 参数/用例错误或没有适用测试。
`make` 会将失败退出码显示成 Error，这是验证失败的正常表现。

## 笔记

### dijkstra

算法编写共2轮 经过ai纠错并改正后通过运行

犯下的错:

1. `FindClosestUnsettled` 返回值 num 和 index 弄混

2. `FindClosestUnsettled` 未处理*排除已经处理结束的节点*的边界情况

3. `if (pre < result.dist[j])` 覆盖最小路径时 未更新前驱

4. `Weight pre = G.arcs[present][j] + result.dist[present];` 变量 pre 错用int

### floyd

算法编写共2轮 经过ai纠错并改正后通过运行

犯下的错:

1. 错误理解了 `result.next` 数组的含义 导致不知道其作用

2. `pre = result.dist[i][k] + result.dist[j][i];` 计算距离时错误使用了 `G.arcs`
