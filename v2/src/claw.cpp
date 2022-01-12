#include "main.h"

int claw_lock = 0;
bool claw_toggle = true;

pros::ADIDigitalOut claw(1);

void set_claw(bool in) { claw.set_value(in); }

void claw_up() {
  set_claw(true);
  claw_toggle = true;
}

void claw_down() {
  set_claw(false);
  claw_toggle = false;
}

void claw_control() {
  if (master.get_digital(DIGITAL_L2) && claw_lock == 0) {
    claw_toggle = !claw_toggle;
    claw_lock = 1;
  } else if (!master.get_digital(DIGITAL_L2)) {
    claw_lock = 0;
  }

  if (claw_toggle)
    claw_up();
  else
    claw_down();
}