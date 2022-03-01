#include "main.h"

Piston dougie_claw('H');
Piston dougie_tilt('F');

void dougie_claw_up() { dougie_claw.set(true); }
void dougie_claw_down() { dougie_claw.set(false); }
void dougie_tilt_up() { dougie_tilt.set(true); }
void dougie_tilt_down() { dougie_tilt.set(false); }

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
  if (master.get_digital_new_press(DIGITAL_R2) && !master.get_digital(DIGITAL_L2)) {
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