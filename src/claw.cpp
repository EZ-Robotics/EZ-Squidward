#include "main.h"

void claw_up() { claw.set(true); }
void claw_down() { claw.set(false); }

void claw_control() {
  claw.button_toggle(master.get_digital(DIGITAL_R1));
}