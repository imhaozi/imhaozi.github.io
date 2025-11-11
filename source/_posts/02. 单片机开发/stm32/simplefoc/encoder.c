#include "encoder.h"
#include "foc_math.h"
#include "main.h"

// 获取电机角度函数（弧度制 负无穷到正无穷）
float get_angle(encoder_ptr encoder)
{
    // 1. 获取传感器角度（0-2PI）
    float angle = encoder->get_sensor_angle();

    // 2. 计算与上次角度差值
    float df_angle = angle - encoder->angle_prev;

    // 3. 计算是否越过了0度（0-2PI-0-2PI-0）过中间这个点时，角度差值会突增
    // 也就是说，两次读取角度之间的时间，不够电机转0.8圈
    if (fabs(df_angle) > (0.8f * _2PI))
        encoder->full_rotation += df_angle > 0 ? -_2PI : _2PI;

    // 4. 更新角度
    encoder->angle_prev = angle;

    // 特殊：防止单方向旋转时间太久之后，浮点数过大
    /*
    if (encoder->full_rotation >= (_2PI * 1000))
    {
        encoder->full_rotation = 0;
        // angle_prev = angle_prev - _2PI * 1000;
    }
    if (encoder->full_rotation <= (-_2PI * 1000))
    {
        encoder->full_rotation = 0;
        // angle_prev = angle_prev + _2PI * 1000;
    }
    */

    // 5. 计算轴角度（整圈数*2PI + 本次角度）
    // 除法中，除数与被除数有任意一个浮点数，则会按照浮点数计算，因此结果为浮点数
    encoder->angle = encoder->full_rotation + angle;

    // 返回
    return encoder->angle;
}

// 获取速度和获取角度 相互独立
float get_velocity(encoder_ptr encoder)
{
    float ts;
    uint32_t now_us;

    // 1. 计算两次调用时间差
    now_us = SysTick->VAL;
    if (now_us < encoder->pre_timestamp)
        ts = (float)(encoder->pre_timestamp - now_us) / encoder->sys_clock_freq * 1e-6f;
    else
        ts = (float)(0xFFFFFF - now_us + encoder->pre_timestamp) / encoder->sys_clock_freq * 1e-6f;
    if (ts == 0 || ts > 0.5f)
        ts = 1e-3f;

    // 2. 获取传感器角度
    float angle = encoder->get_sensor_angle();

    // 3. 计算与上次角度差值
    float df_angle = angle - encoder->velocity_angle_prev;

    // 4. 计算是否越过了0度（0-360-0-360-0）过中间这个点时，数据差值会突增
    // 也就是说，两次读取角度之间的时间，不够电机转0.8圈。
    if (fabs(df_angle) > (0.8f * _2PI))
    {
        if (df_angle < 0) // 说明在正转（读数增加方向）
        {
            // _2PI - old + new = _2PI + (new - old)
            encoder->velocity = (_2PI + df_angle) / ts; // _2PI - old + new
        }
        else // 反转（读数减小方向）
        {
            // _2PI - new + old = _2PI - (new - old)
            encoder->velocity = (_2PI - df_angle) / ts;
        }
    }
    else
    {
        // 速度 = 弧度差 / 时间差
        encoder->velocity = df_angle / ts;
    }

    // 5. 更新参数
    encoder->velocity_angle_prev = angle;
    encoder->pre_timestamp = now_us;

    // 返回
    return encoder->velocity;
}
