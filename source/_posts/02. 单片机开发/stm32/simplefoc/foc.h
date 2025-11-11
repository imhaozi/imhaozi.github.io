#ifndef __FOC_H
#define __FOC_H

#include "stdint.h"
#include "svpwm.h"

// motor rotation direction
typedef enum
{
    CW = 1,   // clockwise
    CCW = -1, // anticlockwise
    NUL = 0   // null
} DIRECTION;

// foc struct
typedef struct _foc
{
    // ctrl input
    float tar_uq;
    float tar_ud;

    // hardware param
    uint8_t pole_pairs;

    float shaft_angle; // shaft_angle
    float e_angle;     // electrical_angle

    DIRECTION sensor_direction; //
    float zero_electric_angle;  // Sensor deviation (sensor reading at motor 0 degrees)

    float (*get_angle)(void);

    svpwm_t svpwm;
    void (*generate_svpwm)(svpwm_ptr svpwm, float tar_uq, float tar_ud, float e_angle);
} foc_t;
typedef foc_t *foc_ptr;

void foc_ctrl(foc_ptr foc, float tar_uq, float tar_ud);

#endif /* __FOC_H */
