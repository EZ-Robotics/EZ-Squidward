#include "main.h"

// Piston creation using EZ-Template Piston class
Piston dougie_claw('H');
Piston dougie_tilt('F');

// Set claw and tilt
void set_dougie_claw_tilt(bool claw, bool tilt) {
  dougie_claw.set(claw);
  dougie_tilt.set(tilt);
}

// Array of dougie states.  First param is claw, second is tilt.
//  Claw Open: true
//  Claw Closed: false
//  Tilt Up: true
//  Tilt Down: false
const int NO_OF_STATES = 3;
bool dougie_state[NO_OF_STATES][2] = {
    {true, false},   // State 1: tilt down and claw open
    {false, false},  // State 2: tilt down and claw closed
    {false, true}    // State 3: tilt up and claw closed
};

// Print claw state to terminal
std::string print_claw_state(bool input) {
  if (input) return "Open";
  return "Closed";
}

// Print tilt state to terminal
std::string print_tilt_state(bool input) {
  if (input) return "Up";
  return "Down";
}

// Set current dougie state
void set_dougie_state(int state) {
  current_dougie_state = state;
  bool claw = dougie_state[current_dougie_state - 1][0];
  bool tilt = dougie_state[current_dougie_state - 1][1];
  set_dougie_claw_tilt(claw, tilt);
  std::cout << "\nNew Dougie State: " << current_dougie_state << " {Claw " << print_claw_state(claw) << ", Dougie " << print_tilt_state(tilt) << "}";
}

// This is ran in usercontrol to control the lift
void dougie_control() {
  // When R1 is pressed and shift (L2) is NOT pressed, increase the current state.
  // Wrap current state around NO_OF_STATES
  if (master.get_digital_new_press(DIGITAL_R2) && !master.get_digital(DIGITAL_L2)) {
    current_dougie_state++;
    if (current_dougie_state > NO_OF_STATES) current_dougie_state = 1;
    set_dougie_state(current_dougie_state);
  }
}