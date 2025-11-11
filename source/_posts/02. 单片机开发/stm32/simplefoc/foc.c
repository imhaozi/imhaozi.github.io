#include "foc.h"
#include "svpwm.h"
#include "foc_math.h"

// set foc loop target
static void set_foc_target(foc_ptr foc, float tar_uq, float tar_ud)
{
    foc->tar_uq = tar_uq;
    foc->tar_ud = tar_ud;
}

// inner loop, outter just need change foc->tar_uq and foc->tar_ud
void foc_ctrl(foc_ptr foc, float tar_uq, float tar_ud)
{
    set_foc_target(foc, tar_uq, tar_ud);

    // update shaft_angle
    foc->shaft_angle = foc->sensor_direction * foc->get_angle();

    // update e_angle
    foc->e_angle = f_normalize(foc->shaft_angle * foc->pole_pairs - foc->zero_electric_angle);

    // core api, important!!!
    foc->generate_svpwm(&(foc->svpwm), foc->tar_uq, foc->tar_ud, foc->e_angle);
}
