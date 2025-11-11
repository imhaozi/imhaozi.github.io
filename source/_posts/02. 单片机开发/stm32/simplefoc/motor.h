#ifndef __MOTOR_H
#define __MOTOR_H

#include "stdint.h"

#include "svpwm.h"
#include "foc.h"
#include "speed.h"
#include "position.h"
#include "encoder.h"

// ctrl_mode
typedef enum
{
    UNKNOW = 0,  //
    TORQUE = 1,  //
    SPEED = 2,   //
    POSITION = 3 //
} MODE;

typedef struct _motor
{
    MODE mode;
    float target;

    foc_t foc;
    speed_t speed;
    position_t position;
    encoder_t encoder;
} motor_t;
typedef motor_t *motor_ptr;

extern motor_t motor;


float read_sensor_angle(void);
void motor_init(motor_ptr motor);
void set_motor(motor_ptr motor, MODE mode, float target);
void motor_run(motor_ptr motor);

#endif /* __MOTOR_H */
