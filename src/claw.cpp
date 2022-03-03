#include "main.h"

// These are wrapper functions that make it easier to modify the state if wiring changes
void claw_up() { claw.set(true); }     // Claw in open state
void claw_down() { claw.set(false); }  // Claw in closed state

// This is ran in usercontrol to control the claw
void claw_control() {
  // One button toggle using R1 when shift key (L2) is not pressed
  if (!master.get_digital(DIGITAL_L2))
    claw.button_toggle(master.get_digital(DIGITAL_R1));
}