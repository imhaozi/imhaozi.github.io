#ifndef __PID_H
#define __PID_H

#include "main.h"
#include "foc_math.h"

typedef struct _pid
{
    float p;                      // 比例
    float i;                      // 积分
    float d;                      // 微分
    float output_ramp;            // 输出斜率限制
    float limit;                  // 输出限制
    float error_prev;             // 上次误差
    float output_prev;            // 上次输出
    float integral_prev;          // 上次积分值
    unsigned long timestamp_prev; // 上次使用时间
    uint16_t sys_clock_freq;      // system clock
} pid_t;
typedef pid_t *pid_ptr;

float pid_operator(pid_ptr pid, float error);

#endif /* __PID_H */
