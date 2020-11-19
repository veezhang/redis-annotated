/* adlist.h - A generic doubly linked list implementation
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ADLIST_H__
#define __ADLIST_H__

/* Node, List, and Iterator are the only data structures used currently. */

//zw 双向链表结点
typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;
} listNode;

//zw 链表迭代器 双向遍历
typedef struct listIter {
    listNode *next;
    int direction;
} listIter;

//zw 双向链表
typedef struct list {
    listNode *head;                                 //zw 头结点
    listNode *tail;                                 //zw 尾节点
    void *(*dup)(void *ptr);                        //zw 复制链表
    void (*free)(void *ptr);                        //zw 释放链表
    int (*match)(void *ptr, void *key);             //zw 匹配
    unsigned long len;                              //zw 长度
} list;

/* Functions implemented as macros */
#define listLength(l) ((l)->len)                    //zw 链表长度
#define listFirst(l) ((l)->head)                    //zw 链表头结点
#define listLast(l) ((l)->tail)                     //zw 链表尾结点
#define listPrevNode(n) ((n)->prev)                 //zw 结点的前一个结点
#define listNextNode(n) ((n)->next)                 //zw 结点的后一个结点
#define listNodeValue(n) ((n)->value)               //zw 结点的值

#define listSetDupMethod(l,m) ((l)->dup = (m))      //zw 设置赋值函数
#define listSetFreeMethod(l,m) ((l)->free = (m))    //zw 设置释放函数
#define listSetMatchMethod(l,m) ((l)->match = (m))  //zw 设置匹配函数

#define listGetDupMethod(l) ((l)->dup)              //zw 获取赋值函数
#define listGetFree(l) ((l)->free)                  //zw 获取释放函数
#define listGetMatchMethod(l) ((l)->match)          //zw 获取匹配函数

/* Prototypes */
list *listCreate(void);                             //zw 创建链表
void listRelease(list *list);                       //zw 释放链表
list *listAddNodeHead(list *list, void *value);     //zw 头添加结点
list *listAddNodeTail(list *list, void *value);     //zw 尾添加结点
list *listInsertNode(list *list, listNode *old_node, void *value, int after);   //zw 在某结点前或后插入结点
void listDelNode(list *list, listNode *node);       //zw 删除结点
listIter *listGetIterator(list *list, int direction);//zw 获取迭代器
listNode *listNext(listIter *iter);                 //zw 迭代器指向下一个
void listReleaseIterator(listIter *iter);           //zw 释放迭代器
list *listDup(list *orig);                          //zw 链表复制
listNode *listSearchKey(list *list, void *key);     //zw 链表查询
listNode *listIndex(list *list, long index);        //zw 链表下标操作, 可以正负
void listRewind(list *list, listIter *li);          //zw 重置迭代器从头开始
void listRewindTail(list *list, listIter *li);      //zw 重置迭代器从尾开始
void listRotate(list *list);                        //zw 旋转链表， 将最后一个放置最前

/* Directions for iterators */
#define AL_START_HEAD 0                             //zw 迭代器从头开始
#define AL_START_TAIL 1                             //zw 迭代器从尾开始

#endif /* __ADLIST_H__ */
