#include "problem.h"
#include <stdlib.h>

/*
 * [设计思想]（4分）请用 2~3 句话简述算法核心思路：
 *   1.
 *
 * [复杂度]（2分）
 *   时间复杂度：O(...)
 *   空间复杂度：O(...)
 */

/**
 * 一开始没理解 后来看了真题解析视频 理解了得用辅助哈希表来算了
 * 但c没有哈希表 所以只能用一个n长度的数组来代替
 * 用一个辅助数组来记录元素是否出现过
 * 若有就删除 若没有就将当前数组当前节点位置置1
 *
 * 时间复杂度 O(n)
 * 空间复杂度 O(m)
 */

void purge_dup_abs(LinkList head, int n) {
    int* has = (int*)calloc(n + 1, sizeof(int));

    LinkList dummyhead = head;
    int presNum;
    while (dummyhead != NULL && dummyhead->next != NULL) {
        presNum = dummyhead->next->data > 0 ? dummyhead->next->data
                                            : -dummyhead->next->data;

        if (has[presNum] != 0) {
            LinkList drop = dummyhead->next;
            dummyhead->next = dummyhead->next->next;
            free(drop);
        } else {
            has[presNum] = 1;
            dummyhead = dummyhead->next;
        }
    }

    free(has);
}
