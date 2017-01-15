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

//zw ����һ���µ�ziplist
unsigned char *ziplistNew(void);
//zw ��zl��pushһ��entry�� ͷ���β��
unsigned char *ziplistPush(unsigned char *zl, unsigned char *s, unsigned int slen, int where);
//zw ����zl���ҵ�index����λ�õ�ͷָ�룬���������Χ�򷵻�NULLָ��, index�����ɸ�
unsigned char *ziplistIndex(unsigned char *zl, int index);
//zw ���뵱ǰentry��ͷָ��p���õ���һ��entry��ͷָ�룬��������ڣ�����NULL
unsigned char *ziplistNext(unsigned char *zl, unsigned char *p);
//zw ����zl�е�һ��entry��ͷָ��p��������ǰһ��Ԫ�ص�ͷָ�룬��������ڣ�����NULL
unsigned char *ziplistPrev(unsigned char *zl, unsigned char *p);
//zw ����entry��ͷָ�룬��ȡ���е����ݣ����pΪNULL������β�ڵ㣬�򷵻�0�����򷵻�1.
unsigned int ziplistGet(unsigned char *p, unsigned char **sval, unsigned int *slen, long long *lval);
//zw ��zl��pλ�ô���Ԫ��s
unsigned char *ziplistInsert(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen);
//zw ɾ��*pָ��entry����������һ��entry��ͷָ��*p
unsigned char *ziplistDelete(unsigned char *zl, unsigned char **p);
//zw ��index����ʼ��ɾ��num��entry
unsigned char *ziplistDeleteRange(unsigned char *zl, unsigned int index, unsigned int num);
//zw ��p��ʼ��entry��������value�ͳ���Ϊslen��sstr�����Ƚ�
unsigned int ziplistCompare(unsigned char *p, unsigned char *s, unsigned int slen);
//zw �ҵ�ziplist�а����������ݵĽڵ㣬û��skip����һ�£�skip=2��0-3-6-9������
unsigned char *ziplistFind(unsigned char *p, unsigned char *vstr, unsigned int vlen, unsigned int skip);
//zw ����zl��������entry����
unsigned int ziplistLen(unsigned char *zl);
//zw �õ�zl����ռ�õ��ڴ��С����λbytes
size_t ziplistBlobLen(unsigned char *zl);
