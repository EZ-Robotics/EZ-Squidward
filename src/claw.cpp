#include "main.h"

pros::ADIDigitalOut claw('G');

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
  if (master.get_digital_new_press(DIGITAL_R1)) {
    claw_state = !claw_state;
    if (claw_state)
      claw_up();
    else
      claw_down();
  }
}