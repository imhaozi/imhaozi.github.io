#ifndef __SVPWM_H
#define __SVPWM_H

#include "stdint.h"

typedef struct _svpwm
{
    // ctrl input
    float tar_uq;
    float tar_ud;
    float e_angle;

    // dont care
    float t0;
    float t1;
    float t2;
    float ta;
    float tb;
    float tc;
    float Uout;
    uint8_t sector;

    // hardware param
    float power_voltage;

    // user needs to provide
    void (*set_a_duty)(float time);
    void (*set_b_duty)(float time);
    void (*set_c_duty)(float time);
} svpwm_t;
typedef svpwm_t *svpwm_ptr;

void svpwm_ctrl(svpwm_ptr svpwm, float tar_uq, float tar_ud, float e_angle);

#endif /* __SVPWM_H */
