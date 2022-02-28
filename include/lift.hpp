#pragma once
#include "api.h"

enum lift_state {
  FAST_DOWN = -1,
  DOWN = 0,
  MID = 260,
  UP = 330
};

void set_lift_exit();
void set_lift_speed(int input);
void set_lift(int input);
void reset_lift();
void set_lift_state(lift_state input);
void wait_lift();

void lift_control();