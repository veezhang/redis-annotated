/*
 * Copyright (c) 2009-2012, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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
 
 
 http://blog.csdn.net/acceptedxukai/article/details/17472055
 */

#ifndef __INTSET_H
#define __INTSET_H
#include <stdint.h>

//zw 底层本质是一个有序的（小到大）、不重复的、整型的数组，支持不同类型整数
typedef struct intset {
    uint32_t encoding;      //zw 所使用类型的长度，4\8\16 
    uint32_t length;        //zw 元素个数
    int8_t contents[];      //zw 保存元素的数组
} intset;

//zw 新建intset
intset *intsetNew(void);
//zw 直接添加元素
intset *intsetAdd(intset *is, int64_t value, uint8_t *success);
//zw 删除元素
intset *intsetRemove(intset *is, int64_t value, int *success);
//zw 查找元素
uint8_t intsetFind(intset *is, int64_t value);
//zw 随机返回一个intset中元素
int64_t intsetRandom(intset *is);
//zw 根据索引获取整数值
uint8_t intsetGet(intset *is, uint32_t pos, int64_t *value);
//zw intset中元素的个数
uint32_t intsetLen(intset *is);
//zw intset所占的字节数
size_t intsetBlobLen(intset *is);

#endif // __INTSET_H
