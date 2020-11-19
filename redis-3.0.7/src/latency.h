/* latency.h -- latency monitor API header file
 * See latency.c for more information.
 *
 * ----------------------------------------------------------------------------
 *
 * Copyright (c) 2014, Salvatore Sanfilippo <antirez at gmail dot com>
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

#ifndef __LATENCY_H
#define __LATENCY_H
//zw latency延迟分析处理

#define LATENCY_TS_LEN 160 /* History length for every monitored event. 监听时间的历史长度，采样点的个数 */

/* Representation of a latency sample: the sampling time and the latency
 * observed in milliseconds. */
//zw 延时采样点
struct latencySample {
    int32_t time; /* We don't use time_t to force 4 bytes usage everywhere. 采样创建时间（毫秒）*/
    uint32_t latency; /* Latency in milliseconds. 延时的具体时间（毫秒）*/
};

/* The latency time series for a given event. */
//zw 针对某个事件采集的一系列延时采样点
struct latencyTimeSeries {
    int idx; /* Index of the next sample to store. 下一个延时采样点的下标*/
    uint32_t max; /* Max latency observed for this event. 最大的延时*/
    struct latencySample samples[LATENCY_TS_LEN]; /* Latest history. 历史延时采样点*/
};

/* Latency statistics structure. */
//zw 延时采样数据统计结果
struct latencyStats {
    uint32_t all_time_high; /* Absolute max observed since latest reset. 绝对最高的延时时间*/
    uint32_t avg;           /* Average of current samples. 平均延时时间*/
    uint32_t min;           /* Min of current samples. 最小延时时间*/
    uint32_t max;           /* Max of current samples. 最大延时时间*/
    uint32_t mad;           /* Mean absolute deviation. 平均相对误差，与平均延时相比*/
    uint32_t samples;       /* Number of non-zero samples. 采样点的数目*/
    time_t period;          /* Number of seconds since first event and now. 最早的延时记录点的创建时间 与当前时间的间隔*/
};

//zw 延时监听初始化操作， 创建dict
void latencyMonitorInit(void);
//zw 添加延时采样点
void latencyAddSample(char *event, mstime_t latency);
//zw 是否支持透明大页(Transparent Huge Pages)
int THPIsEnabled(void);

/* Latency monitoring macros. */

/* Start monitoring an event. We just set the current time. */
//zw 对某个事件设置监听，就是设置一下当前的时间
#define latencyStartMonitor(var) if (server.latency_monitor_threshold) { \
    var = mstime(); \
} else { \
    var = 0; \
}

/* End monitoring an event, compute the difference with the current time
 * to check the amount of time elapsed. */
//zw 结束监听，算出过了多少时间
#define latencyEndMonitor(var) if (server.latency_monitor_threshold) { \
    var = mstime() - var; \
}

/* Add the sample only if the elapsed time is >= to the configured threshold. */
//zw 如果延时时间超出server.latency_monitor_threshold，则将Sample加入延时列表中
#define latencyAddSampleIfNeeded(event,var) \
    if (server.latency_monitor_threshold && \
        (var) >= server.latency_monitor_threshold) \
          latencyAddSample((event),(var));

/* Remove time from a nested event. */
//zw 移除其他签到在里面的时间， 这里用加法，是将起始时间向后移，最终耗时减少
#define latencyRemoveNestedEvent(event_var,nested_var) \
    event_var += nested_var;

#endif /* __LATENCY_H */
