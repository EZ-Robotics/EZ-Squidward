#include "main.h"

int claw_lock = 0;

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

void claw_control() {
  if (master.get_digital(DIGITAL_L2) && claw_lock == 0) {
    claw_state = !claw_state;
    claw_lock = 1;
  } else if (!master.get_digital(DIGITAL_L2)) {
    claw_lock = 0;
  }

  if (claw_state)
    claw_up();
  else
    claw_down();
}