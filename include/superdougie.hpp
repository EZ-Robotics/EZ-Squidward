#pragma once

inline bool dougie_claw_state = true;
inline bool dougie_tilt_state = true;
inline int current_state = 1;

void dougie_claw_up();
void dougie_claw_down();

void dougie_tilt_up();
void dougie_tilt_down();

void sd_state_1();
void sd_state_2();
void sd_state_3();

void dougie_control();