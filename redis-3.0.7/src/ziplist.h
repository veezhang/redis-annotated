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
 */

/* zw
http://blog.csdn.net/acceptedxukai/article/details/17426321
http://www.cnblogs.com/igloo1986/archive/2012/08/30/2662972.html
http://www.programering.com/a/MjM3QDMwATY.html
*/

#define ZIPLIST_HEAD 0
#define ZIPLIST_TAIL 1

//zw 创建一个新的ziplist
unsigned char *ziplistNew(void);
//zw 向zl中push一个entry， 头插或尾插
unsigned char *ziplistPush(unsigned char *zl, unsigned char *s, unsigned int slen, int where);
//zw 遍历zl，找到index所在位置的头指针，如果超出范围则返回NULL指针, index可正可负
unsigned char *ziplistIndex(unsigned char *zl, int index);
//zw 输入当前entry的头指针p，得到下一个entry的头指针，如果不存在，返回NULL
unsigned char *ziplistNext(unsigned char *zl, unsigned char *p);
//zw 输入zl中的一个entry的头指针p，返回其前一个元素的头指针，如果不存在，返回NULL
unsigned char *ziplistPrev(unsigned char *zl, unsigned char *p);
//zw 根据entry的头指针，提取其中的内容，如果p为NULL或者是尾节点，则返回0，否则返回1.
unsigned int ziplistGet(unsigned char *p, unsigned char **sval, unsigned int *slen, long long *lval);
//zw 在zl的p位置存入元素s
unsigned char *ziplistInsert(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen);
//zw 删除*p指向entry，并返回下一个entry的头指针*p
unsigned char *ziplistDelete(unsigned char *zl, unsigned char **p);
//zw 从index，开始，删除num个entry
unsigned char *ziplistDeleteRange(unsigned char *zl, unsigned int index, unsigned int num);
//zw 把p开始的entry所包含的value和长度为slen的sstr串作比较
unsigned int ziplistCompare(unsigned char *p, unsigned char *s, unsigned int slen);
//zw 找到ziplist中包含给定数据的节点，没过skip个找一下，skip=2，0-3-6-9。。。
unsigned char *ziplistFind(unsigned char *p, unsigned char *vstr, unsigned int vlen, unsigned int skip);
//zw 返回zl所包含的entry数量
unsigned int ziplistLen(unsigned char *zl);
//zw 得到zl整个占用的内存大小，单位bytes
size_t ziplistBlobLen(unsigned char *zl);
