#include "svpwm.h"
#include "foc_math.h"

// it's different from Odrive
void svpwm_ctrl(svpwm_ptr svpwm, float tar_uq, float tar_ud, float e_angle)
{
    svpwm->tar_uq = tar_uq;
    svpwm->tar_ud = tar_ud;
    svpwm->e_angle = e_angle;

    if (svpwm->tar_ud) // only if svpwm->tar_ud and svpwm->tar_uq set
    {
        svpwm->Uout = f_sqrt(svpwm->tar_ud * svpwm->tar_ud + svpwm->tar_uq * svpwm->tar_uq) / svpwm->power_voltage;
        svpwm->e_angle = f_normalize(svpwm->e_angle + atan2(svpwm->tar_uq, svpwm->tar_ud));
    }
    else
    {
        svpwm->Uout = svpwm->tar_uq / svpwm->power_voltage;
        svpwm->e_angle = f_normalize(svpwm->e_angle + _PI_2);
    }
    if (svpwm->Uout > 0.577f)
        svpwm->Uout = 0.577f;
    if (svpwm->Uout < -0.577f)
        svpwm->Uout = -0.577f;

    svpwm->sector = (svpwm->e_angle / _PI_3) + 1;

    svpwm->t1 = _SQRT3 * f_sin(svpwm->sector * _PI_3 - svpwm->e_angle) * svpwm->Uout;
    svpwm->t2 = _SQRT3 * f_sin(svpwm->e_angle - (svpwm->sector - 1.0f) * _PI_3) * svpwm->Uout;
    svpwm->t0 = 1 - svpwm->t1 - svpwm->t2;

    // calculate the duty cycles(times)
    switch (svpwm->sector)
    {
    case 1:
        svpwm->ta = svpwm->t1 + svpwm->t2 + svpwm->t0 / 2;
        svpwm->tb = svpwm->t2 + svpwm->t0 / 2;
        svpwm->tc = svpwm->t0 / 2;
        break;
    case 2:
        svpwm->ta = svpwm->t1 + svpwm->t0 / 2;
        svpwm->tb = svpwm->t1 + svpwm->t2 + svpwm->t0 / 2;
        svpwm->tc = svpwm->t0 / 2;
        break;
    case 3:
        svpwm->ta = svpwm->t0 / 2;
        svpwm->tb = svpwm->t1 + svpwm->t2 + svpwm->t0 / 2;
        svpwm->tc = svpwm->t2 + svpwm->t0 / 2;
        break;
    case 4:
        svpwm->ta = svpwm->t0 / 2;
        svpwm->tb = svpwm->t1 + svpwm->t0 / 2;
        svpwm->tc = svpwm->t1 + svpwm->t2 + svpwm->t0 / 2;
        break;
    case 5:
        svpwm->ta = svpwm->t2 + svpwm->t0 / 2;
        svpwm->tb = svpwm->t0 / 2;
        svpwm->tc = svpwm->t1 + svpwm->t2 + svpwm->t0 / 2;
        break;
    case 6:
        svpwm->ta = svpwm->t1 + svpwm->t2 + svpwm->t0 / 2;
        svpwm->tb = svpwm->t0 / 2;
        svpwm->tc = svpwm->t1 + svpwm->t0 / 2;
        break;
    default: // possible error state
        svpwm->ta = 0;
        svpwm->tb = 0;
        svpwm->tc = 0;
    }

    svpwm->set_a_duty(svpwm->ta);
    svpwm->set_b_duty(svpwm->tb);
    svpwm->set_c_duty(svpwm->tc);
}
