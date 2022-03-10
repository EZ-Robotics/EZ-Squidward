#include "main.h"

using namespace ez;

// Variables used lower in the code
const int SLOW_SPEED = 80;
const int FAST_SPEED = 127;

// PID creation using EZ-Template PID class
PID r_liftPID{4, 0, 0, 0, "Right Lift"};
PID l_liftPID{4, 0, 0, 0, "Left Lift"};
PID equalizerPID{1, 0, 3, 0, "Lift Equalizer"};  // Makes sure the left and right side go the same speed up
bool is_equalizing = true;

// Motor creation
pros::Motor r_lift_motor(1, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
pros::Motor l_lift_motor(10, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);

// Max speed the lift can travel at
int lift_max_speed = 127;
void set_lift_speed(int input) { lift_max_speed = abs(input); }

// Set lift to voltage
void set_lift(int left, int right) {
  r_lift_motor = right;
  l_lift_motor = left;
}

// Reset lift sensors
void reset_lift() {
  l_lift_motor.tare_position();
  r_lift_motor.tare_position();
}

// Exit conditions for PID to know when it's arrived at destination
void set_lift_exit() {
  r_liftPID.set_exit_condition(80, 20, 300, 50, 500, 500);
  l_liftPID.set_exit_condition(80, 20, 300, 50, 500, 500);
}

// Prints current state of lift to terminal
std::string lift_state_to_string(lift_state input) {
  switch (input) {
    case DOWN:
      return "Down";
      break;
    case FAST_DOWN:
      return "Fast Down";
      break;
    case SLIGHT_RAISE:
      return "Slight Raise";
      break;
    case RIGHT_TWIST:
      return "Right Twist";
      break;
    case LEFT_TWIST:
      return "Left Twist";
      break;
    case MID:
      return "Mid";
      break;
    case UP:
      return "Up";
      break;
    default:
      return "Out of bounds lift state";
      break;
  }
}

// Set lift state.  This makes setting the lift to specific positions easier,
// using words instead of numbers and dealing with specific logic for each state.
void set_lift_state(lift_state input) {
  set_lift_speed(current_lift_state > input ? SLOW_SPEED : FAST_SPEED);
  current_lift_state = input;
  if (input == LEFT_TWIST) {
    r_liftPID.set_target(0);
    l_liftPID.set_target(LEFT_TWIST);
    is_equalizing = false;
  } else if (input == RIGHT_TWIST) {
    r_liftPID.set_target(RIGHT_TWIST);
    l_liftPID.set_target(0);
    is_equalizing = false;
  } else {
    r_liftPID.set_target(input);
    l_liftPID.set_target(input);
    is_equalizing = true;
  }
  std::cout << "\nNew Lift State: " << lift_state_to_string(input);
}

// This function always runs in the background and deals with controlling the lift.
void liftTask() {
  double l_output = 0, r_output = 0;
  long timer = 0;
  bool did_reset = false;
  while (true) {
    // Current left and right sensors
    double l_current = l_lift_motor.get_position();
    double r_current = r_lift_motor.get_position();

    // Set current to left current
    equalizerPID.set_target(l_current);

    // Computes PID and clips the speed to max speed
    double l_clipped_pid = util::clip_num(l_liftPID.compute(l_current), lift_max_speed, -lift_max_speed);
    double r_clipped_pid = util::clip_num(r_liftPID.compute(r_current), lift_max_speed, -lift_max_speed);
    double equalizer_output = is_equalizing ? equalizerPID.compute(r_current) : 0;  // Compute using right current

    // Instead of using PID to come down, the robot will set the lift to some power and when the velocity of the motor is 0
    // (the motor is at the bottom), will reset the encoders so the PID will continue to work.
    if (current_lift_state == DOWN || current_lift_state == FAST_DOWN) {
      if (l_current >= 20 || r_current >= 20) {
        l_output = l_clipped_pid;
        r_output = r_clipped_pid;
      } else {
        bool check = ((r_lift_motor.get_actual_velocity() == 0 || l_lift_motor.get_actual_velocity() == 0) && !pros::competition::is_disabled()) ? true : false;
        if (check) timer += util::DELAY_TIME;
        if (timer >= 250) {
          l_output = -4;
          r_output = -4;
          if (!did_reset) reset_lift();
          did_reset = true;
          timer = 250;
        } else {
          int speed = current_lift_state == DOWN ? -50 : -100;
          l_output = speed;
          r_output = speed;
        }
      }
    } else {
      timer = 0;
      did_reset = false;
      l_output = l_clipped_pid;
      r_output = r_clipped_pid;
    }

    if (pros::competition::is_disabled()) timer = 0;

    // Set lift to output, but speed up the slow side / slow down the fast side
    // set_lift(l_output - equalizer_output, r_output + equalizer_output);
    set_lift(l_output, r_output);

    pros::delay(util::DELAY_TIME);
  }
}
pros::Task lift_task(liftTask);

// Blocking function for the lift during autonomous
void wait_lift() {
  while (r_liftPID.exit_condition(r_lift_motor, true) == ez::RUNNING) {
    pros::delay(ez::util::DELAY_TIME);
  }
}

// This is ran in usercontrol to control the lift
void lift_control() {
  // L1 is a toggle for full up / down
  if (master.get_digital_new_press(DIGITAL_L1)) {
    if (current_lift_state == UP || current_lift_state == MID || current_lift_state == SLIGHT_RAISE || current_lift_state == LEFT_TWIST || current_lift_state == RIGHT_TWIST)
      set_lift_state(DOWN);
    else
      set_lift_state(UP);
  }

  // If the shift key (L2) and toggle are pressed, lift will go to middle height
  if (master.get_digital(DIGITAL_L1) && master.get_digital_new_press(DIGITAL_L2))
    set_lift_state(MID);

  // Twisty right
  else if (master.get_digital_new_press(DIGITAL_A))
    set_lift_state(RIGHT_TWIST);

  // Twisty left
  else if (master.get_digital_new_press(DIGITAL_LEFT))
    set_lift_state(LEFT_TWIST);
}