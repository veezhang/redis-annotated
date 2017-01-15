/* SDSLib, A C dynamic strings library
 *
 * Copyright (c) 2006-2010, Salvatore Sanfilippo <antirez at gmail dot com>
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

#ifndef __SDS_H
#define __SDS_H

#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>

//zw Ϊchar *���Ͷ������Ϊsds
typedef char *sds;

//zw �ַ����ṹ��
struct sdshdr {
    //zw �ַ�����ǰ����
    unsigned int len;
    //zw ʣ����ó���
    unsigned int free;
    //zw �ַ�����
    char buf[];
};

//zw ��ȡ�ַ�������
static inline size_t sdslen(const sds s) {
    //zw ��s-(sizeof(struct sdshdr))�͵õ�sdshdr *ָ��
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->len;
}

//zw ��ȡ�ַ������еĿ��ÿռ�
static inline size_t sdsavail(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->free;
}

//zw ���ݸ���ʼ������init�͸������ȴ����µ��ַ���
sds sdsnewlen(const void *init, size_t initlen);
//zw ���ݸ�����ֵ����sds
sds sdsnew(const char *init);
//zw ����һ�����ַ���
sds sdsempty(void);
//zw ��ȡsds����
size_t sdslen(const sds s);
//zw ����һ��sds
sds sdsdup(const sds s);
//zw �ͷ�sds���ڴ�ռ�
void sdsfree(sds s);
//zw ��ȡsds�Ŀ��ÿռ�
size_t sdsavail(const sds s);
//zw ��չ�ַ�����ָ���ĳ���
sds sdsgrowzero(sds s, size_t len);
//zw �ַ������Ӳ���
sds sdscatlen(sds s, const void *t, size_t len);
sds sdscat(sds s, const char *t);
sds sdscatsds(sds s, const sds t);
//zw �ַ������Ʋ���
sds sdscpylen(sds s, const char *t, size_t len);
sds sdscpy(sds s, const char *t);

//zw �ַ���׷�� ��ʽ�����ַ���
sds sdscatvprintf(sds s, const char *fmt, va_list ap);
#ifdef __GNUC__
sds sdscatprintf(sds s, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
sds sdscatprintf(sds s, const char *fmt, ...);
#endif
//zw ������sdscatvprintf�� �������
sds sdscatfmt(sds s, char const *fmt, ...);
//zw �ַ�����trim�������߼������ձ��ṩ��ȥ��ǰ���cset���ַ�
sds sdstrim(sds s, const char *cset);
//zw �ַ�����ȡ
void sdsrange(sds s, int start, int end);
//zw �����ַ����ĳ���
void sdsupdatelen(sds s);
//zw ����ַ���
void sdsclear(sds s);
//zw �ַ����Ƚϲ���
int sdscmp(const sds s1, const sds s2);
//zw �ַ����ָ����
sds *sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count);
//zw �ͷ�sdssplitlen�������ص�sds����
void sdsfreesplitres(sds *tokens, int count);
//zw תСд
void sdstolower(sds s);
//zw ת��д
void sdstoupper(sds s);
//zw ��һ��long long���͵�����ת��Ϊ�ַ���
sds sdsfromlonglong(long long value);
//zw ���ַ���תΪ���Ű������ַ�����ת���ַ���ԭ���ɼ������Ǵ�ӡ�ַ�ת16����
sds sdscatrepr(sds s, const char *p, size_t len);
//zw ��������
sds *sdssplitargs(const char *line, int *argc);
//zw �ַ��滻����
sds sdsmapchars(sds s, const char *from, const char *to, size_t setlen);
//zw ��һ��C�����ַ���������ָ���ָ������ӳ�һ���ַ���
sds sdsjoin(char **argv, int argc, char *sep);

/* Low level functions exposed to the user API */
//zw  ȷ��sds�еĿ��ÿռ���ڻ����addlen�������ǰ�ַ������ÿռ䲻�������������ÿռ�
sds sdsMakeRoomFor(sds s, size_t addlen);
//zw ���ݸ�������incr������ǰ���ȺͿ��ÿռ��С
void sdsIncrLen(sds s, int incr);
//zw �ͷ��ַ�����buf�еĶ���ռ䣬ʹ��պ��ܴ�ŵ�ǰ�ַ���
sds sdsRemoveFreeSpace(sds s);
//zw ��ȡsdsʵ�ʷ���Ŀռ��С����������'\0'��������
size_t sdsAllocSize(sds s);

#endif
