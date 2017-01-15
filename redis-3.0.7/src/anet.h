/* anet.c -- Basic TCP socket stuff made a bit less boring
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

#ifndef ANET_H
#define ANET_H

#define ANET_OK 0
#define ANET_ERR -1
#define ANET_ERR_LEN 256

/* Flags used with certain functions. */
#define ANET_NONE 0
#define ANET_IP_ONLY (1<<0)

#if defined(__sun) || defined(_AIX)
#define AF_LOCAL AF_UNIX
#endif

#ifdef _AIX
#undef ip_len
#endif

//zw TCP��Ĭ������
int anetTcpConnect(char *err, char *addr, int port);
//zw TCP�ķ���������
int anetTcpNonBlockConnect(char *err, char *addr, int port);

int anetTcpNonBlockBindConnect(char *err, char *addr, int port, char *source_addr);

int anetTcpNonBlockBestEffortBindConnect(char *err, char *addr, int port, char *source_addr);
//zw anet��Unix��ʽ��Ĭ�����ӷ�ʽ
int anetUnixConnect(char *err, char *path);
//zw anet��Unix��ʽ�ķ��������ӷ�ʽ
int anetUnixNonBlockConnect(char *err, char *path);
//zw anet�����ȡ�ļ���buffer�в���
int anetRead(int fd, char *buf, int count);
//zw �������еĶ���
int anetResolve(char *err, char *host, char *ipbuf, size_t ipbuf_len);
//zw ��������IP�ĵ�ַ
int anetResolveIP(char *err, char *host, char *ipbuf, size_t ipbuf_len);
//zw anet TCP IP4 Server
int anetTcpServer(char *err, int port, char *bindaddr, int backlog);
//zw anet TCP IP6 Server
int anetTcp6Server(char *err, int port, char *bindaddr, int backlog);
//zw anet Unix Server
int anetUnixServer(char *err, char *path, mode_t perm, int backlog);
//zw anet TCP Accept
int anetTcpAccept(char *err, int serversock, char *ip, size_t ip_len, int *port);
//zw anet Unix Accept
int anetUnixAccept(char *err, int serversock);
//zw anetͨ�������buffer��д���ļ�����
int anetWrite(int fd, char *buf, int count);
//zw anet���÷������ķ���
int anetNonBlock(char *err, int fd);
//zw anet���������ķ���
int anetBlock(char *err, int fd);
//zw ����TCPû���ӳ�
int anetEnableTcpNoDelay(char *err, int fd);
//zw ����TCP����û���ӳ�
int anetDisableTcpNoDelay(char *err, int fd);
//zw ����TCP���ֻ�Ծ����״̬������������ϵͳ
int anetTcpKeepAlive(char *err, int fd);
//zw ���÷��ͳ�ʱʱ��
int anetSendTimeout(char *err, int fd, long long ms);
//zw ����fd ��ȡԶ��socket ip��port
int anetPeerToString(int fd, char *ip, size_t ip_len, int *port);
//zw ����TCP����һֱ����������Ѿ���ȥ�Ľ�㣬intervalѡ��ֻ������Linux�µ�ϵͳ
int anetKeepAlive(char *err, int fd, int interval);
//zw ����fd ��ȡ����socket ip��port
int anetSockName(int fd, char *ip, size_t ip_len, int *port);

#endif
