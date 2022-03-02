#include "main.h"

// These are wrapper functions that make it easier to modify the state if wiring changes
void shtick_up() { shtick.set(false); }   // Shtick in up position
void shtick_down() { shtick.set(true); }  // Shtick in down position

// This is ran in usercontrol to control the claw
void shtick_control() {
  // This is a button toggle with R2 only when the shift key (L2) is pressed
  if (master.get_digital(DIGITAL_L2))
    shtick.button_toggle(master.get_digital(DIGITAL_R2));
}