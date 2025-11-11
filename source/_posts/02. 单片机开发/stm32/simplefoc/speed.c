#include "speed.h"
#include "main.h"

// speed low pass
static float velocity_lpf_ctrl(lpf_ptr lpf, float x)
{
    float ts;
    uint32_t now_us;
    float alpha, y;

    // 1. calculate the time difference between two calls
    now_us = SysTick->VAL;
    if (now_us < lpf->pre_timestamp)
        ts = (float)(lpf->pre_timestamp - now_us) / lpf->sys_clock_freq * 1e-6f;
    else
        ts = (float)(0xFFFFFF - now_us + lpf->pre_timestamp) / lpf->sys_clock_freq * 1e-6f;

    // 2. update
    lpf->pre_timestamp = now_us;

    // 3. if the time difference is significant, output directly
    if (ts > 0.3f)
    {
        lpf->y_prev = x;
        return x;
    }

    // 4. if the time difference is not significant, filtering is required
    alpha = lpf->tf / (lpf->tf + ts);
    y = alpha * lpf->y_prev + (1.0f - alpha) * x;
    lpf->y_prev = y;

    return y;
}

// set speed loop target
static void set_speed_target(speed_ptr speed, float tar_speed)
{
    speed->tar_speed = tar_speed;
}

// speed loop
float speed_ctrl(speed_ptr speed, float tar_speed)
{
    set_speed_target(speed, tar_speed);

    // get speed
    float velocity = speed->get_velocity();

    // lpf
    velocity = velocity_lpf_ctrl(&(speed->lpf), velocity);

    // pid TODO：voltage loop -> current loop
    return pid_operator(&(speed->pid), speed->tar_speed - velocity);
}
