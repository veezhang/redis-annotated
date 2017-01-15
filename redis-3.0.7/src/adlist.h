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

//zw ˫��������
typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;
} listNode;

//zw ��������� ˫�����
typedef struct listIter {
    listNode *next;
    int direction;
} listIter;

//zw ˫������
typedef struct list {
    listNode *head;                                 //zw ͷ���
    listNode *tail;                                 //zw β�ڵ�
    void *(*dup)(void *ptr);                        //zw ��������
    void (*free)(void *ptr);                        //zw �ͷ�����
    int (*match)(void *ptr, void *key);             //zw ƥ��
    unsigned long len;                              //zw ����
} list;

/* Functions implemented as macros */
#define listLength(l) ((l)->len)                    //zw ������
#define listFirst(l) ((l)->head)                    //zw ����ͷ���
#define listLast(l) ((l)->tail)                     //zw ����β���
#define listPrevNode(n) ((n)->prev)                 //zw ����ǰһ�����
#define listNextNode(n) ((n)->next)                 //zw ���ĺ�һ�����
#define listNodeValue(n) ((n)->value)               //zw ����ֵ

#define listSetDupMethod(l,m) ((l)->dup = (m))      //zw ���ø�ֵ����
#define listSetFreeMethod(l,m) ((l)->free = (m))    //zw �����ͷź���
#define listSetMatchMethod(l,m) ((l)->match = (m))  //zw ����ƥ�亯��

#define listGetDupMethod(l) ((l)->dup)              //zw ��ȡ��ֵ����
#define listGetFree(l) ((l)->free)                  //zw ��ȡ�ͷź���
#define listGetMatchMethod(l) ((l)->match)          //zw ��ȡƥ�亯��

/* Prototypes */
list *listCreate(void);                             //zw ��������
void listRelease(list *list);                       //zw �ͷ�����
list *listAddNodeHead(list *list, void *value);     //zw ͷ��ӽ��
list *listAddNodeTail(list *list, void *value);     //zw β��ӽ��
list *listInsertNode(list *list, listNode *old_node, void *value, int after);   //zw ��ĳ���ǰ��������
void listDelNode(list *list, listNode *node);       //zw ɾ�����
listIter *listGetIterator(list *list, int direction);//zw ��ȡ������
listNode *listNext(listIter *iter);                 //zw ������ָ����һ��
void listReleaseIterator(listIter *iter);           //zw �ͷŵ�����
list *listDup(list *orig);                          //zw ������
listNode *listSearchKey(list *list, void *key);     //zw �����ѯ
listNode *listIndex(list *list, long index);        //zw �����±����, ��������
void listRewind(list *list, listIter *li);          //zw ���õ�������ͷ��ʼ
void listRewindTail(list *list, listIter *li);      //zw ���õ�������β��ʼ
void listRotate(list *list);                        //zw ��ת���� �����һ��������ǰ

/* Directions for iterators */
#define AL_START_HEAD 0                             //zw ��������ͷ��ʼ
#define AL_START_TAIL 1                             //zw ��������β��ʼ

#endif /* __ADLIST_H__ */
