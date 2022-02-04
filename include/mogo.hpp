#pragma once
#include "api.h"

namespace m {
enum mogo_state {
  DOWN = 600, //560
  UP = 65, // 80
};
}

void set_mogo_exit();
void set_mogo_speed(int input);
void set_mogo(int input);
void reset_mogo();
void set_mogo_state(m::mogo_state input);
void wait_mogo();

void mogo_control();

void zero_mogo();