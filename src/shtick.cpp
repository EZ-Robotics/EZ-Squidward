#include "main.h"

void shtick_up() { shtick.set(false); }
void shtick_down() { shtick.set(true); }

void shtick_control() {
  if (master.get_digital(DIGITAL_L2))
    shtick.button_toggle(master.get_digital(DIGITAL_R2));
}