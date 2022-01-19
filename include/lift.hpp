#pragma once
#include "api.h"

enum lift_state {
  DOWN = 0,
  MID = 460,
  UP = 700
};

void set_lift_exit();
void set_lift_speed(int input);
void set_lift(int input);
void reset_lift();
void set_lift_state(lift_state input);
void wait_lift();

void lift_control();