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
//zw 定义文件事件处理接口（函数指针）
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
//zw 时间事件处理接口（函数指针），该函数返回定时的时长
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
//zw 在超时事件中注册，用于在删除时间事件后执行
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
//zw aeMain中使用，在调用处理事件前调用
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

//zw 文件事件结构体
/* File event structure */
typedef struct aeFileEvent {
    //zw 事件类型
    int mask; /* one of AE_(READABLE|WRITABLE) */
    //zw 读事件的处理函数
    aeFileProc *rfileProc;
    //zw 写事件的处理函数
    aeFileProc *wfileProc;
    //zw 传递给上述两个函数的数据
    void *clientData;
} aeFileEvent;

//zw 时间事件结构体
/* Time event structure */
typedef struct aeTimeEvent {
    //zw 时间事件标识符，用于唯一标识该时间事件
    long long id; /* time event identifier. */
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */
    //zw 事件对应的处理程序
    aeTimeProc *timeProc;
    //zw 时间事件的最后一次处理程序，若已设置，则删除时间事件时会被调用
    aeEventFinalizerProc *finalizerProc;
    void *clientData;
    //zw 下一个时间事件
    struct aeTimeEvent *next;
} aeTimeEvent;

//zw 保存已触发的事件
/* A fired event */
typedef struct aeFiredEvent {
    int fd;
    int mask;
} aeFiredEvent;

/* State of an event based program */
typedef struct aeEventLoop {
    //zw 当前注册最大文件描述符
    int maxfd;   /* highest file descriptor currently registered */
    //zw 文件描述符最大监听数
    int setsize; /* max number of file descriptors tracked */
    //zw 用于生成时间事件的唯一标识id
    long long timeEventNextId;
    //zw 用于检测系统时间是否变更（判断标准 now<lastTime）
    time_t lastTime;     /* Used to detect system clock skew */
    //zw 注册要使用的文件事件，这里的分离表实现为直接索引，即通过fd来访问，实现事件的分离
    aeFileEvent *events; /* Registered events */
    //zw 已触发的事件
    aeFiredEvent *fired; /* Fired events */
    //zw 时间事件
    aeTimeEvent *timeEventHead;
    //zw 停止标志
    int stop;
    //zw 这个是处理底层特定API的数据，对于epoll来说，该结构体包含了epoll fd和epoll_event
    void *apidata; /* This is used for polling API specific data */
    //zw 在调用processEvent前（即如果没有事件则睡眠），调用该处理函数
    aeBeforeSleepProc *beforesleep;
} aeEventLoop;

/* Prototypes */
//zw 创建事件循环，setsize为最大事件的的个数，对于epoll来说也是epoll_event的个数
aeEventLoop *aeCreateEventLoop(int setsize);
//zw 删除事件循环
void aeDeleteEventLoop(aeEventLoop *eventLoop);
//zw 停止事件循环
void aeStop(aeEventLoop *eventLoop);
//zw 创建文件事件，并将该事件注册到eventLoop中
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData);
//zw 删除文件事件
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);
//zw 获取文件事件mask
int aeGetFileEvents(aeEventLoop *eventLoop, int fd);
//zw 创建时间事件
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
//zw 删除时间事件
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);
//zw 事件处理程序
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
//zw 等待事件产生
int aeWait(int fd, int mask, long long milliseconds);
//zw 主循环
void aeMain(aeEventLoop *eventLoop);
//zw 获取API名字，如epoll返回epoll
char *aeGetApiName(void);
//zw 设置beforesleep处理函数
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);
//zw 获取监听数
int aeGetSetSize(aeEventLoop *eventLoop);
//zw 重设监听数
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif
