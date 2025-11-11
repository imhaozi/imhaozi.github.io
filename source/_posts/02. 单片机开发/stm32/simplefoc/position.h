#ifndef __POSITION_H
#define __POSITION_H

#include "pid.h"

typedef struct _position
{
    // ctrl input
    float tar_position;

    pid_t pid;

    // user needs to provide
    float (*get_angle)(void);
} position_t;
typedef position_t *position_ptr;

float position_ctrl(position_ptr position, float tar_position);

#endif /* __POSITION_H */
