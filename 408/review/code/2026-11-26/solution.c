#include "problem.h"
#include <stdlib.h>

/*
 * [设计思想]（4分）请用 2~3 句话简述算法核心思路：
 * 自己想到了用双指针left从0开始 right从n-p开始 right迭代条件为(right++) % 8
 * 但是直到right迭代到0结束 但被移动到后方的几个元素总是顺序会乱
 * 所以决定去看解析动画尝试理解
 *
 * 动画看完了 比我想象的要简单 将数组分为前p个数和后n-p个数两个子数组
 * 先分别reverse 再整体reverse
 *
 * 时间复杂度 O(n)
 * 空间复杂度 O(1)
 *
 * [复杂度]（2分）
 *   时间复杂度：O(...)
 *   空间复杂度：O(...)
 */
void reverse(int arr[], int left, int right) {
    while (left <= right) {
        int tmp = arr[right];
        arr[right] = arr[left];
        arr[left] = tmp;

        left++;
        right--;
    }
}

void left_shift(int R[], int n, int p) {
    /* TODO: 请使用纯 C 语言实现（勿使用 C++ 语法或库） */
    reverse(R, 0, p - 1);
    reverse(R, p, n - 1);
    reverse(R, 0, n - 1);
}
