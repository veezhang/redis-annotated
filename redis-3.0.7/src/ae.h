/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
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

#ifndef __AE_H__
#define __AE_H__

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4

#define AE_NOMORE -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

struct aeEventLoop;

/* Types and data structures */
//zw �����ļ��¼�����ӿڣ�����ָ�룩
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
//zw ʱ���¼�����ӿڣ�����ָ�룩���ú������ض�ʱ��ʱ��
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
//zw �ڳ�ʱ�¼���ע�ᣬ������ɾ��ʱ���¼���ִ��
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
//zw aeMain��ʹ�ã��ڵ��ô����¼�ǰ����
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

//zw �ļ��¼��ṹ��
/* File event structure */
typedef struct aeFileEvent {
    //zw �¼�����
    int mask; /* one of AE_(READABLE|WRITABLE) */
    //zw ���¼��Ĵ�����
    aeFileProc *rfileProc;
    //zw д�¼��Ĵ�����
    aeFileProc *wfileProc;
    //zw ���ݸ�������������������
    void *clientData;
} aeFileEvent;

//zw ʱ���¼��ṹ��
/* Time event structure */
typedef struct aeTimeEvent {
    //zw ʱ���¼���ʶ��������Ψһ��ʶ��ʱ���¼�
    long long id; /* time event identifier. */
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */
    //zw �¼���Ӧ�Ĵ������
    aeTimeProc *timeProc;
    //zw ʱ���¼������һ�δ�������������ã���ɾ��ʱ���¼�ʱ�ᱻ����
    aeEventFinalizerProc *finalizerProc;
    void *clientData;
    //zw ��һ��ʱ���¼�
    struct aeTimeEvent *next;
} aeTimeEvent;

//zw �����Ѵ������¼�
/* A fired event */
typedef struct aeFiredEvent {
    int fd;
    int mask;
} aeFiredEvent;

/* State of an event based program */
typedef struct aeEventLoop {
    //zw ��ǰע������ļ�������
    int maxfd;   /* highest file descriptor currently registered */
    //zw �ļ���������������
    int setsize; /* max number of file descriptors tracked */
    //zw ��������ʱ���¼���Ψһ��ʶid
    long long timeEventNextId;
    //zw ���ڼ��ϵͳʱ���Ƿ������жϱ�׼ now<lastTime��
    time_t lastTime;     /* Used to detect system clock skew */
    //zw ע��Ҫʹ�õ��ļ��¼�������ķ����ʵ��Ϊֱ����������ͨ��fd�����ʣ�ʵ���¼��ķ���
    aeFileEvent *events; /* Registered events */
    //zw �Ѵ������¼�
    aeFiredEvent *fired; /* Fired events */
    //zw ʱ���¼�
    aeTimeEvent *timeEventHead;
    //zw ֹͣ��־
    int stop;
    //zw ����Ǵ���ײ��ض�API�����ݣ�����epoll��˵���ýṹ�������epoll fd��epoll_event
    void *apidata; /* This is used for polling API specific data */
    //zw �ڵ���processEventǰ�������û���¼���˯�ߣ������øô�����
    aeBeforeSleepProc *beforesleep;
} aeEventLoop;

/* Prototypes */
//zw �����¼�ѭ����setsizeΪ����¼��ĵĸ���������epoll��˵Ҳ��epoll_event�ĸ���
aeEventLoop *aeCreateEventLoop(int setsize);
//zw ɾ���¼�ѭ��
void aeDeleteEventLoop(aeEventLoop *eventLoop);
//zw ֹͣ�¼�ѭ��
void aeStop(aeEventLoop *eventLoop);
//zw �����ļ��¼����������¼�ע�ᵽeventLoop��
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData);
//zw ɾ���ļ��¼�
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);
//zw ��ȡ�ļ��¼�mask
int aeGetFileEvents(aeEventLoop *eventLoop, int fd);
//zw ����ʱ���¼�
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
//zw ɾ��ʱ���¼�
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);
//zw �¼��������
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
//zw �ȴ��¼�����
int aeWait(int fd, int mask, long long milliseconds);
//zw ��ѭ��
void aeMain(aeEventLoop *eventLoop);
//zw ��ȡAPI���֣���epoll����epoll
char *aeGetApiName(void);
//zw ����beforesleep������
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);
//zw ��ȡ������
int aeGetSetSize(aeEventLoop *eventLoop);
//zw ���������
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif
