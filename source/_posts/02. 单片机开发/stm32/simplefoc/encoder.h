#ifndef __ENCODER_H
#define __ENCODER_H

#include "stdint.h"

// foc结构体
typedef struct _encoder
{
    float angle;    // 角度
    float velocity; // 速度

    // 计算角度相关
    float full_rotation;
    float angle_prev;

    // 计算速度相关
    uint32_t pre_timestamp;
    uint32_t velocity_angle_prev;

    uint16_t sys_clock_freq;

    // 获取传感器角度（0-2PI）的浮点数
    float (*get_sensor_angle)(void);
} encoder_t;
typedef encoder_t *encoder_ptr;

float get_angle(encoder_ptr encoder);
float get_velocity(encoder_ptr encoder);

#endif /* __ENCODER_H */
