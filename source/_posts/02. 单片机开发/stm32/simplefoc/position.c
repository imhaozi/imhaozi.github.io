#include "position.h"

// set position loop target
static void set_position_target(position_ptr position, float tar_position)
{
    position->tar_position = tar_position;
}

// position loop
float position_ctrl(position_ptr position, float tar_position)
{
    set_position_target(position, tar_position);

    // get position
    float angle = position->get_angle();

    // pid
    return pid_operator(&(position->pid), position->tar_position - angle);
}
