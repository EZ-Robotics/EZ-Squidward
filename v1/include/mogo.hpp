#pragma once

extern bool b_mogo_up;

void set_mogo(int input);
void zero_mogo(int input = 0);
int get_mogo();
int get_mogo_vel();
void set_mogo_position(int target, int speed);

bool timeout(int target, int vel, int current);
void mogo_up  (bool hold = false);
void mogo_down(bool hold = false);
void mogo_mid (bool hold = false);

void mogo_control();
