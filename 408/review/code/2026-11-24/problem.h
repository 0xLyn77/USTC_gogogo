#ifndef PROBLEM_H
#define PROBLEM_H

#include <stdbool.h>
#include <stddef.h>

/*
 * 题目来源：[真题原题 · 2015年 第41题]（15分）
 *
 * 题目描述：
 *   用带头结点的单链表保存 m 个整数，结点结构为 [ data | next ]，
 *   且对链表中每个有效数据结点满足 |data| <= n（n 为正整数）。
 *   现要求设计一个时间复杂度尽可能高效的算法，对于链表中 data 的绝对值相等的结点，
 *   仅保留第一次出现的结点，而删除其余绝对值相等的结点（并释放被删结点空间）。
 *
 * 结构体与接口契约说明：
 *   1. 单链表带头结点（dummy head）：head 指向头结点，head->data 不存放有效业务数据，
 *      首元结点为 head->next（空表时 head->next == NULL）。
 *   2. 参数 n 为正整数（n >= 1），保证所有有效结点的 |data| <= n（注意 data 可取 0、n 或 -n）。
 *   3. 原地修改单链表：保留首次出现的原结点本身（勿重新分配结点替代原保留结点），
 *      被删结点须调用 free() 释放内存，并确保处理后表尾结点 next == NULL。
 *   4. 若 head == NULL 或链表为空表，函数应安全返回。
 */

typedef struct LNode {
    int data;
    struct LNode *next;
} LNode, *LinkList;

/* 目标函数原型 */
void purge_dup_abs(LinkList head, int n);

#endif /* PROBLEM_H */
