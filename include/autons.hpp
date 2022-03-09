#pragma once

#include "EZ-Template/drive/drive.hpp"

extern Drive chassis;

enum lift_movement { NONE = 0,
                     L_TWIST = 1,
                     R_TWIST = 2,
                     RAISE = 3
};

void default_constants();

void double_steal();

void plat_up_center_raw(lift_movement input);
void plat_up_center();
void plat_up_center_l_twist();
void plat_up_center_r_twist();
void plat_up_center_raise();

void plat_down_center_raw(lift_movement input);
void plat_down_center();
void plat_down_center_l_twist();
void plat_down_center_r_twist();
void plat_down_center_raise();