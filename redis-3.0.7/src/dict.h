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
//zw 消除未使用变量警告
#define DICT_NOTUSED(V) ((void) V)

//zw 字典实体 保存key-value值
typedef struct dictEntry {
    void *key;
    union {                         //联合体，可以存不同的数据类型
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    } v;
    struct dictEntry *next;         //zw 下一个
} dictEntry;

//zw 字典类型， 一簇函数
typedef struct dictType {
    unsigned int (*hashFunction)(const void *key);                                  //zw hash 方法
    void *(*keyDup)(void *privdata, const void *key);                               //zw key值复制
    void *(*valDup)(void *privdata, const void *obj);                               //zw value值复制
    int (*keyCompare)(void *privdata, const void *key1, const void *key2);          //zw key值比较
    void (*keyDestructor)(void *privdata, void *key);                               //zw key值析构
    void (*valDestructor)(void *privdata, void *obj);                               //zw value值析构
} dictType;

/* This is our hash table structure. Every dictionary has two of this as we
 * implement incremental rehashing, for the old to the new table. */
//zw 哈希表结构 dict hash table
typedef struct dictht {
    dictEntry **table;              //zw 字典实体 保存key-value值，可以看做字典数组，俗称桶bucket
    unsigned long size;             //zw 指针数组的大小，即桶的数量， 这里为2的N次方
    unsigned long sizemask;         //zw mask 码，用于地址索引计算 size-1, hash值与sizemask取与
    unsigned long used;             //zw 字典中当前的节点数目
} dictht;

//zw 字典结构
typedef struct dict {
    dictType *type;         //zw 字典类型， 一簇函数
    void *privdata;         //zw 私有数据指针
    dictht ht[2];           //zw 
    long rehashidx; /* rehashing not in progress if rehashidx == -1 指示 rehash 是否正在进行，如果不是则为 -1*/
    int iterators; /* number of iterators currently running 当前正在使用的 iterator 的数量*/
} dict;

/* If safe is set to 1 this is a safe iterator, that means, you can call
 * dictAdd, dictFind, and other functions against the dictionary even while
 * iterating. Otherwise it is a non safe iterator, and only dictNext()
 * should be called while iterating. */
/*zw 字典迭代器，如果是安全迭代器，这safe设置为1，可以调用dicAdd，dictFind 
如果是不安全的，则只能调用dicNext方法
将safe置1可将迭代器定义为安全的，也就是说添加、查找或者其他操作是与其他的迭代器排他的。
如果是非安全的，则只能进行遍历操作
*/
typedef struct dictIterator {
    dict *d;                            //zw 当前字典
    long index;                         //zw 下标
    int table, safe;                    //zw 表格，和安全值，表格代表的是旧的表格，还是新的表格 
    dictEntry *entry, *nextEntry;       //zw 字典实体
    /* unsafe iterator fingerprint for misuse detection. */
    long long fingerprint;              //zw 指纹标记，避免不安全的迭代器滥用现象
} dictIterator;

//zw 字典扫描方法
typedef void (dictScanFunction)(void *privdata, const dictEntry *de);

/* This is the initial size of every hash table */
#define DICT_HT_INITIAL_SIZE     4

/* ------------------------------- Macros ------------------------------------*/
//zw 字典释放value
#define dictFreeVal(d, entry) \
    if ((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->v.val)

//zw 字典设置value
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

//zw 字典释放key
#define dictFreeKey(d, entry) \
    if ((d)->type->keyDestructor) \
        (d)->type->keyDestructor((d)->privdata, (entry)->key)

//zw 字典设置key
#define dictSetKey(d, entry, _key_) do { \
    if ((d)->type->keyDup) \
        entry->key = (d)->type->keyDup((d)->privdata, _key_); \
    else \
        entry->key = (_key_); \
} while(0)

//zw key比较
#define dictCompareKeys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
        (d)->type->keyCompare((d)->privdata, key1, key2) : \
        (key1) == (key2))

//zw 哈希定位方法
#define dictHashKey(d, key) (d)->type->hashFunction(key)
//zw 获取dictEntry的key值
#define dictGetKey(he) ((he)->key)
//zw 获取dicEntry中共用体v中定义的val值 
#define dictGetVal(he) ((he)->v.val)
#define dictGetSignedIntegerVal(he) ((he)->v.s64)
#define dictGetUnsignedIntegerVal(he) ((he)->v.u64)
#define dictGetDoubleVal(he) ((he)->v.d)
//zw 获取dict字典中总的表大小
#define dictSlots(d) ((d)->ht[0].size+(d)->ht[1].size)
//zw 获取dict字典中总的表的总正在被使用的数量
#define dictSize(d) ((d)->ht[0].used+(d)->ht[1].used)
//zw 字典有无被重定位过
#define dictIsRehashing(d) ((d)->rehashidx != -1)

/* API */
//zw 创建dict字典
dict *dictCreate(dictType *type, void *privDataPtr);
//zw 字典扩增方法
int dictExpand(dict *d, unsigned long size);
//zw 字典根据key, val添加一个字典集
int dictAdd(dict *d, void *key, void *val);
//zw 字典添加一个只有key值的dicEntry
dictEntry *dictAddRaw(dict *d, void *key);
//zw 替代dict中一个字典集
int dictReplace(dict *d, void *key, void *val);
//zw 替代dict中的一个字典，只提供一个key值
dictEntry *dictReplaceRaw(dict *d, void *key);
//zw 根据key删除一个字典集
int dictDelete(dict *d, const void *key);
//zw 字典集删除无、不调用free方法
int dictDeleteNoFree(dict *d, const void *key);
//zw 释放整个dict
void dictRelease(dict *d);
//zw 根据key寻找字典集 
dictEntry * dictFind(dict *d, const void *key);
//zw 根据key值寻找相应的val值 
void *dictFetchValue(dict *d, const void *key);
//zw 重新计算大小
int dictResize(dict *d);
//zw 获取字典迭代器
dictIterator *dictGetIterator(dict *d);
//zw 获取字典安全迭代器
dictIterator *dictGetSafeIterator(dict *d);
//zw 根据字典迭代器获取字典集的下一字典集 
dictEntry *dictNext(dictIterator *iter);
//zw 释放迭代器
void dictReleaseIterator(dictIterator *iter);
//zw 随机获取一个字典集
dictEntry *dictGetRandomKey(dict *d);
//zw 随机获取多个字典集
unsigned int dictGetSomeKeys(dict *d, dictEntry **des, unsigned int count);
//zw 打印当前字典状态
void dictPrintStats(dict *d);
//zw 输入的key值，目标长度，此方法帮你计算hash值
unsigned int dictGenHashFunction(const void *key, int len);
//zw 这里提供了一种比较简单的哈希算法
unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len);
//zw 清空字典
void dictEmpty(dict *d, void(callback)(void*));
//zw 启用调整方法
void dictEnableResize(void);
//zw 禁用调整方法
void dictDisableResize(void);
//zw hash重定位，主要从旧的表映射到新表中,分n轮定位
int dictRehash(dict *d, int n);
//zw 在给定时间内，循环执行哈希重定位
int dictRehashMilliseconds(dict *d, int ms);
//zw 设置哈希方法种子
void dictSetHashFunctionSeed(unsigned int initval);
//zw 获取哈希种子
unsigned int dictGetHashFunctionSeed(void);
//zw 字典扫描方法  
unsigned long dictScan(dict *d, unsigned long v, dictScanFunction *fn, void *privdata);

/* Hash table types */
//zw 哈希表类型
extern dictType dictTypeHeapStringCopyKey;
extern dictType dictTypeHeapStrings;
extern dictType dictTypeHeapStringCopyKeyValue;

#endif /* __DICT_H */
