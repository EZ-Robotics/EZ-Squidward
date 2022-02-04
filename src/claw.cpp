#include "main.h"

pros::ADIDigitalOut claw(1);

void set_claw(bool in) { claw.set_value(in); }

void claw_up() {
  set_claw(true);
  claw_state = true;
}

void claw_down() {
  set_claw(false);
  claw_state = false;
}

int last_l2 = 0;
void claw_control() {
  if (master.get_digital(DIGITAL_L2) && last_l2 == 0) {
    claw_state = !claw_state;
    if (claw_state) 
      claw_up();
    else 
      claw_down();
  }
  last_l2 = master.get_digital(DIGITAL_L2);
}