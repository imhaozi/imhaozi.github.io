#include "motor.h"
#include "tim.h"

#include "mt6701_dev.h"

/* =========================== need user change start =========================== */
void SET_PWM_A_Duty(float duty)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty * 999); // func need user change_1
}

void SET_PWM_B_Duty(float duty)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty * 999); // func need user change_2
}

void SET_PWM_C_Duty(float duty)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, duty * 999); // func need user change_3
}

mt6701_t mt6701 =
{
	.port = &hi2c1,
};

float read_sensor_angle(void)
{
    return MT6701_GET_Angle(&mt6701); // func need user change_4
}

float get_motor_angle(void)
{
    return get_angle(&(motor.encoder));
}

float get_motor_velocity(void)
{
    return get_velocity(&(motor.encoder));
}

motor_t motor =
    {
        /* foc part */
        .foc.svpwm.tar_uq = 0,
        .foc.svpwm.tar_ud = 0,
        .foc.svpwm.power_voltage = 5, // need user change_1
        .foc.svpwm.set_a_duty = SET_PWM_A_Duty,
        .foc.svpwm.set_b_duty = SET_PWM_B_Duty,
        .foc.svpwm.set_c_duty = SET_PWM_C_Duty,

        .foc.tar_uq = 0,
        .foc.tar_ud = 0,
        .foc.pole_pairs = 7,          // need user change_2
        .foc.sensor_direction = NUL,  // need user change_3 or use init auto set
        .foc.zero_electric_angle = 0, // need user change_4 or use init auto set
        .foc.get_angle = get_motor_angle,
        .foc.generate_svpwm = svpwm_ctrl,

        /* speed part */
        .speed.lpf.tf = 0.0001, // need user change_5
        .speed.lpf.y_prev = 0,
        .speed.lpf.pre_timestamp = 0,
        .speed.lpf.sys_clock_freq = 72, // need user change_6

        .speed.pid.p = 0.1, // need user change_7
        .speed.pid.i = 1,   // need user change_8
        .speed.pid.d = 0,   // need user change_9
        .speed.pid.output_ramp = 50,
        .speed.pid.limit = 5 / 1.732, // need user change_10, power / 1.732
        .speed.pid.error_prev = 0,
        .speed.pid.output_prev = 0,
        .speed.pid.integral_prev = 0,
        .speed.pid.timestamp_prev = 0,
		.speed.pid.sys_clock_freq = 72,
        .speed.tar_speed = 0,
        .speed.get_velocity = get_motor_velocity,

        /* position part */
        .position.pid.p = 20, // need user change_11
        .position.pid.i = 0,
        .position.pid.d = 0,
        .position.pid.output_ramp = 0,
        .position.pid.limit = 0,
        .position.pid.error_prev = 0,
        .position.pid.output_prev = 0,
        .position.pid.integral_prev = 0,
        .position.pid.timestamp_prev = 0,
		.position.pid.sys_clock_freq = 72,
        .position.tar_position = 0,
        .position.get_angle = get_motor_angle,

        /* encoder part */
        .encoder.angle = 0,
        .encoder.velocity = 0,
        .encoder.full_rotation = 0,
        .encoder.angle_prev = 0,
        .encoder.pre_timestamp = 0,
        .encoder.velocity_angle_prev = 0,
        .encoder.sys_clock_freq = 72,                 // need user change_12
        .encoder.get_sensor_angle = read_sensor_angle, // need user change_13

        /* ctrl part */
        .mode = POSITION,
        .target = 0,
};
/* =========================== need user change end =========================== */

void motor_init(motor_ptr motor)
{
    // test motor direction and zero pole points
}

void set_motor(motor_ptr motor, MODE mode, float target)
{
    motor->mode = mode;
    motor->target = target;
}

void motor_run(motor_ptr motor)
{
    // avoid modifying target values during the control process
    float temp_target = motor->target;

    // if the pattern is an inner layer, the outer layer will not be executed
    // if the pattern is an outer layer, the inner layer will be executed

    while (1)
    {
        switch (motor->mode)
        {
        case POSITION:
            temp_target = position_ctrl(&(motor->position), temp_target);
        case SPEED:
            temp_target = speed_ctrl(&(motor->speed), temp_target);
        case TORQUE:
            foc_ctrl(&(motor->foc), temp_target, 0);
            break;
        default:
            printf("mode error! \r\n");
            return;
        }
    }
}
