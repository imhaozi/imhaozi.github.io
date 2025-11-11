#ifndef __SPEED_H
#define __SPEED_H

#include "stdint.h"
#include "pid.h"

typedef struct _lpf
{
    float tf;                    // Low pass filter time constant
    float y_prev;                // filtered value in previous execution step
    unsigned long pre_timestamp; // Last execution timestamp
    uint16_t sys_clock_freq;     // system clock
} lpf_t;
typedef lpf_t *lpf_ptr;

typedef struct _speed
{
    // ctrl input
    float tar_speed;

    lpf_t lpf;

    pid_t pid;

    float (*get_velocity)(void);
} speed_t;
typedef speed_t *speed_ptr;

float speed_ctrl(speed_ptr speed, float tar_speed);

#endif /* __SPEED_H */
