#pragma once

extern bool b_lift_up;

void set_lift(int input);
void zero_lift();
int get_lift();
int get_lift_vel();
void set_lift_position(int target, int speed);

bool timeout(int target, int vel, int current);
void lift_down(bool hold = false);
void lift_lil_down(bool hold = false);
void lift_mid(bool hold = false);
void lift_up(bool hold = false);

void lift_control();
