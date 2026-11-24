#ifndef PROBLEM_H
#define PROBLEM_H

#include <stdbool.h>
#include <stddef.h>

/*
 * 题目来源：[填入来源标签]
 * 结构体与接口契约说明：
 *   - 请在此明确数据结构细节（如：单链表是否带头结点、下标从 0 还是 1 开始等）
 */

#define MAXSIZE 100

typedef struct {
    int data[MAXSIZE];
    int length;
} SqList;

/* 目标函数原型 */
bool solve(SqList *L);

#endif /* PROBLEM_H */
