/* Hash Tables Implementation.
 *
 * This file implements in-memory hash tables with insert/del/replace/find/
 * get-random-element operations. Hash tables will auto-resize if needed
 * tables of power of two in size are used, collisions are handled by
 * chaining. See the source code for more information... :)
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

//zw http://blog.csdn.net/moxiaomomo/article/details/22475391
//zw http://blog.csdn.net/androidlushangderen/article/details/39860693

#include <stdint.h>

#ifndef __DICT_H
#define __DICT_H

#define DICT_OK 0
#define DICT_ERR 1

/* Unused arguments generate annoying warnings... */
//zw ����δʹ�ñ�������
#define DICT_NOTUSED(V) ((void) V)

//zw �ֵ�ʵ�� ����key-valueֵ
typedef struct dictEntry {
    void *key;
    union {                         //�����壬���Դ治ͬ����������
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    } v;
    struct dictEntry *next;         //zw ��һ��
} dictEntry;

//zw �ֵ����ͣ� һ�غ���
typedef struct dictType {
    unsigned int (*hashFunction)(const void *key);                                  //zw hash ����
    void *(*keyDup)(void *privdata, const void *key);                               //zw keyֵ����
    void *(*valDup)(void *privdata, const void *obj);                               //zw valueֵ����
    int (*keyCompare)(void *privdata, const void *key1, const void *key2);          //zw keyֵ�Ƚ�
    void (*keyDestructor)(void *privdata, void *key);                               //zw keyֵ����
    void (*valDestructor)(void *privdata, void *obj);                               //zw valueֵ����
} dictType;

/* This is our hash table structure. Every dictionary has two of this as we
 * implement incremental rehashing, for the old to the new table. */
//zw ��ϣ��ṹ dict hash table
typedef struct dictht {
    dictEntry **table;              //zw �ֵ�ʵ�� ����key-valueֵ�����Կ����ֵ����飬�׳�Ͱbucket
    unsigned long size;             //zw ָ������Ĵ�С����Ͱ�������� ����Ϊ2��N�η�
    unsigned long sizemask;         //zw mask �룬���ڵ�ַ�������� size-1, hashֵ��sizemaskȡ��
    unsigned long used;             //zw �ֵ��е�ǰ�Ľڵ���Ŀ
} dictht;

//zw �ֵ�ṹ
typedef struct dict {
    dictType *type;         //zw �ֵ����ͣ� һ�غ���
    void *privdata;         //zw ˽������ָ��
    dictht ht[2];           //zw 
    long rehashidx; /* rehashing not in progress if rehashidx == -1 ָʾ rehash �Ƿ����ڽ��У����������Ϊ -1*/
    int iterators; /* number of iterators currently running ��ǰ����ʹ�õ� iterator ������*/
} dict;

/* If safe is set to 1 this is a safe iterator, that means, you can call
 * dictAdd, dictFind, and other functions against the dictionary even while
 * iterating. Otherwise it is a non safe iterator, and only dictNext()
 * should be called while iterating. */
/*zw �ֵ������������ǰ�ȫ����������safe����Ϊ1�����Ե���dicAdd��dictFind 
����ǲ���ȫ�ģ���ֻ�ܵ���dicNext����
��safe��1�ɽ�����������Ϊ��ȫ�ģ�Ҳ����˵��ӡ����һ��������������������ĵ����������ġ�
����Ƿǰ�ȫ�ģ���ֻ�ܽ��б�������
*/
typedef struct dictIterator {
    dict *d;                            //zw ��ǰ�ֵ�
    long index;                         //zw �±�
    int table, safe;                    //zw ��񣬺Ͱ�ȫֵ����������Ǿɵı�񣬻����µı�� 
    dictEntry *entry, *nextEntry;       //zw �ֵ�ʵ��
    /* unsafe iterator fingerprint for misuse detection. */
    long long fingerprint;              //zw ָ�Ʊ�ǣ����ⲻ��ȫ�ĵ�������������
} dictIterator;

//zw �ֵ�ɨ�跽��
typedef void (dictScanFunction)(void *privdata, const dictEntry *de);

/* This is the initial size of every hash table */
#define DICT_HT_INITIAL_SIZE     4

/* ------------------------------- Macros ------------------------------------*/
//zw �ֵ��ͷ�value
#define dictFreeVal(d, entry) \
    if ((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->v.val)

//zw �ֵ�����value
#define dictSetVal(d, entry, _val_) do { \
    if ((d)->type->valDup) \
        entry->v.val = (d)->type->valDup((d)->privdata, _val_); \
    else \
        entry->v.val = (_val_); \
} while(0)

#define dictSetSignedIntegerVal(entry, _val_) \
    do { entry->v.s64 = _val_; } while(0)

#define dictSetUnsignedIntegerVal(entry, _val_) \
    do { entry->v.u64 = _val_; } while(0)

#define dictSetDoubleVal(entry, _val_) \
    do { entry->v.d = _val_; } while(0)

//zw �ֵ��ͷ�key
#define dictFreeKey(d, entry) \
    if ((d)->type->keyDestructor) \
        (d)->type->keyDestructor((d)->privdata, (entry)->key)

//zw �ֵ�����key
#define dictSetKey(d, entry, _key_) do { \
    if ((d)->type->keyDup) \
        entry->key = (d)->type->keyDup((d)->privdata, _key_); \
    else \
        entry->key = (_key_); \
} while(0)

//zw key�Ƚ�
#define dictCompareKeys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
        (d)->type->keyCompare((d)->privdata, key1, key2) : \
        (key1) == (key2))

//zw ��ϣ��λ����
#define dictHashKey(d, key) (d)->type->hashFunction(key)
//zw ��ȡdictEntry��keyֵ
#define dictGetKey(he) ((he)->key)
//zw ��ȡdicEntry�й�����v�ж����valֵ 
#define dictGetVal(he) ((he)->v.val)
#define dictGetSignedIntegerVal(he) ((he)->v.s64)
#define dictGetUnsignedIntegerVal(he) ((he)->v.u64)
#define dictGetDoubleVal(he) ((he)->v.d)
//zw ��ȡdict�ֵ����ܵı��С
#define dictSlots(d) ((d)->ht[0].size+(d)->ht[1].size)
//zw ��ȡdict�ֵ����ܵı�������ڱ�ʹ�õ�����
#define dictSize(d) ((d)->ht[0].used+(d)->ht[1].used)
//zw �ֵ����ޱ��ض�λ��
#define dictIsRehashing(d) ((d)->rehashidx != -1)

/* API */
//zw ����dict�ֵ�
dict *dictCreate(dictType *type, void *privDataPtr);
//zw �ֵ���������
int dictExpand(dict *d, unsigned long size);
//zw �ֵ����key, val���һ���ֵ伯
int dictAdd(dict *d, void *key, void *val);
//zw �ֵ����һ��ֻ��keyֵ��dicEntry
dictEntry *dictAddRaw(dict *d, void *key);
//zw ���dict��һ���ֵ伯
int dictReplace(dict *d, void *key, void *val);
//zw ���dict�е�һ���ֵ䣬ֻ�ṩһ��keyֵ
dictEntry *dictReplaceRaw(dict *d, void *key);
//zw ����keyɾ��һ���ֵ伯
int dictDelete(dict *d, const void *key);
//zw �ֵ伯ɾ���ޡ�������free����
int dictDeleteNoFree(dict *d, const void *key);
//zw �ͷ�����dict
void dictRelease(dict *d);
//zw ����keyѰ���ֵ伯 
dictEntry * dictFind(dict *d, const void *key);
//zw ����keyֵѰ����Ӧ��valֵ 
void *dictFetchValue(dict *d, const void *key);
//zw ���¼����С
int dictResize(dict *d);
//zw ��ȡ�ֵ������
dictIterator *dictGetIterator(dict *d);
//zw ��ȡ�ֵ䰲ȫ������
dictIterator *dictGetSafeIterator(dict *d);
//zw �����ֵ��������ȡ�ֵ伯����һ�ֵ伯 
dictEntry *dictNext(dictIterator *iter);
//zw �ͷŵ�����
void dictReleaseIterator(dictIterator *iter);
//zw �����ȡһ���ֵ伯
dictEntry *dictGetRandomKey(dict *d);
//zw �����ȡ����ֵ伯
unsigned int dictGetSomeKeys(dict *d, dictEntry **des, unsigned int count);
//zw ��ӡ��ǰ�ֵ�״̬
void dictPrintStats(dict *d);
//zw �����keyֵ��Ŀ�곤�ȣ��˷����������hashֵ
unsigned int dictGenHashFunction(const void *key, int len);
//zw �����ṩ��һ�ֱȽϼ򵥵Ĺ�ϣ�㷨
unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len);
//zw ����ֵ�
void dictEmpty(dict *d, void(callback)(void*));
//zw ���õ�������
void dictEnableResize(void);
//zw ���õ�������
void dictDisableResize(void);
//zw hash�ض�λ����Ҫ�Ӿɵı�ӳ�䵽�±���,��n�ֶ�λ
int dictRehash(dict *d, int n);
//zw �ڸ���ʱ���ڣ�ѭ��ִ�й�ϣ�ض�λ
int dictRehashMilliseconds(dict *d, int ms);
//zw ���ù�ϣ��������
void dictSetHashFunctionSeed(unsigned int initval);
//zw ��ȡ��ϣ����
unsigned int dictGetHashFunctionSeed(void);
//zw �ֵ�ɨ�跽��  
unsigned long dictScan(dict *d, unsigned long v, dictScanFunction *fn, void *privdata);

/* Hash table types */
//zw ��ϣ������
extern dictType dictTypeHeapStringCopyKey;
extern dictType dictTypeHeapStrings;
extern dictType dictTypeHeapStringCopyKeyValue;

#endif /* __DICT_H */
