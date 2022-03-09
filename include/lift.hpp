#pragma once
#include "api.h"

enum lift_state {
  DOWN = 0,
  FAST_DOWN = 1,
  SLIGHT_RAISE = 75,
  RIGHT_TWIST = 150,
  LEFT_TWIST = 150+1,
  MID = 260,
  UP = 330
};

inline lift_state current_lift_state;

void set_lift_exit();
void set_lift_speed(int input);
void set_lift(int input);
void reset_lift();
void set_lift_state(lift_state input);
void wait_lift();

void lift_control();