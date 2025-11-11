#include "pid.h"
#include "foc_math.h"

// pid controller function
float pid_operator(pid_ptr pid, float error)
{
    unsigned long now_us;
    float dt;
    float proportional, integral, derivative, output;
    float output_rate;

    now_us = SysTick->VAL;
    if (now_us < pid->timestamp_prev)
        dt = (float)(pid->timestamp_prev - now_us) / pid->sys_clock_freq * 1e-6f;
    else
        dt = (float)(0xFFFFFF - now_us + pid->timestamp_prev) / pid->sys_clock_freq * 1e-6f;
    pid->timestamp_prev = now_us;
    if (dt == 0 || dt > 0.5f)
        dt = 1e-3f;

    /* u(s) = (P + I/s + Ds)e(s) */

    // u_pk  = P * e(k)
    proportional = pid->p * error;

    // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
    integral = pid->integral_prev + pid->i * dt * 0.5f * (error + pid->error_prev);
    integral = f_constrain(integral, -pid->limit, pid->limit);

    // u_dk = D(ek - ek_1) / Ts
    derivative = pid->d * (error - pid->error_prev) / dt;

    output = proportional + integral + derivative;
    output = f_constrain(output, -pid->limit, pid->limit);

    // if output ramp defined
    if (pid->output_ramp > 0)
    {
        // limit the acceleration by ramping the output
        output_rate = (output - pid->output_prev) / dt;
        if (output_rate > pid->output_ramp)
            output = pid->output_prev + pid->output_ramp * dt;
        else if (output_rate < -pid->output_ramp)
            output = pid->output_prev - pid->output_ramp * dt;
    }

    // saving for the next pass
    pid->integral_prev = integral;
    pid->output_prev = output;
    pid->error_prev = error;

    return output;
}
