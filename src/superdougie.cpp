#include "main.h"

pros::ADIDigitalOut dougie_claw('H');
pros::ADIDigitalOut dougie_tilt('F');

void set_dougie_claw(bool in) { dougie_claw.set_value(in); }
void set_dougie_tilt(bool in) { dougie_tilt.set_value(in); }

void dougie_claw_up() {
  set_dougie_claw(true);
  dougie_claw_state = true;
}

void dougie_claw_down() {
  set_dougie_claw(false);
  dougie_claw_state = false;
}

void dougie_tilt_up() {
  set_dougie_tilt(true);
  dougie_tilt_state = true;
}

void dougie_tilt_down() {
  set_dougie_tilt(false);
  dougie_tilt_state = false;
}

void sd_state_1() {
  current_state = 1;
  dougie_claw_up();
  dougie_tilt_down();
}

void sd_state_2() {
  current_state = 2;
  dougie_claw_down();
  dougie_tilt_down();
}
void sd_state_3() {
  current_state = 3;
  dougie_claw_down();
  dougie_tilt_up();
}

void dougie_control() {
  if (master.get_digital_new_press(DIGITAL_R2)) {
    current_state++;
    if (current_state >= 4) current_state = 1;
  }
  
  switch (current_state) {
    case 1:
      sd_state_1();
      break;
    case 2:
      sd_state_2();
      break;
    case 3:
      sd_state_3();
      break;
    default:
      break;
  }
}