#include "main.h"
#include "pros/adi.hpp"

using namespace ez;

const int SLOW_SPEED = 80;
const int FAST_SPEED = 127;

PID r_liftPID{4, 0, 0, 0, "Right Lift"};
PID l_liftPID{4, 0, 0, 0, "Left Lift"};
pros::Motor r_lift_motor(1, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
pros::Motor l_lift_motor(10, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);
int lift_max_speed = 127;
void set_lift_speed(int input) { lift_max_speed = abs(input); }

void set_lift(int left, int right) {
  r_lift_motor = right;
  l_lift_motor = left;
}

void reset_lift() {
  l_lift_motor.tare_position();
  r_lift_motor.tare_position();
}

void set_lift_exit() {
  r_liftPID.set_exit_condition(80, 20, 300, 50, 500, 500);
  l_liftPID.set_exit_condition(80, 20, 300, 50, 500, 500);
}

std::string lift_state_to_string(lift_state input) {
  switch (input) {
    case FAST_DOWN:
      return "Fast Down";
      break;
    case DOWN:
      return "Down";
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

lift_state current_lift_state;
void set_lift_state(lift_state input) {
  set_lift_speed(current_lift_state > input ? SLOW_SPEED : FAST_SPEED);
  current_lift_state = input;
  r_liftPID.set_target(input);
  l_liftPID.set_target(input);
  std::cout << "\nNew Lift State: " << lift_state_to_string(input);
}

void liftTask() {
  double l_output = 0, r_output = 0;
  long timer = 0;
  bool did_reset = false;
  while (true) {
    double l_current = l_lift_motor.get_position();
    double r_current = r_lift_motor.get_position();

    double l_clipped_pid = util::clip_num(l_liftPID.compute(l_current), lift_max_speed, -lift_max_speed);
    double r_clipped_pid = util::clip_num(r_liftPID.compute(r_current), lift_max_speed, -lift_max_speed);

    if (current_lift_state == DOWN || current_lift_state == FAST_DOWN) {
      if (l_current >= 10 || r_current >= 10) {
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
          if (current_lift_state == DOWN) {
            l_output = -50;
            r_output = -50;
          } else {
            l_output = -100;
            r_output = -100;
          }
        }
      }
    } else {
      timer = 0;
      did_reset = false;
      l_output = l_clipped_pid;
      r_output = r_clipped_pid;
    }

    if (pros::competition::is_disabled()) timer = 0;

    set_lift(l_output, r_output);

    pros::delay(util::DELAY_TIME);
  }
}
pros::Task lift_task(liftTask);

void wait_lift() {
  while (r_liftPID.exit_condition(r_lift_motor, true) == ez::RUNNING) {
    pros::delay(ez::util::DELAY_TIME);
  }
}

void lift_control() {
  if (master.get_digital_new_press(DIGITAL_L1)) {
    if (current_lift_state == UP || current_lift_state == MID)
      set_lift_state(DOWN);
    else
      set_lift_state(UP);
  }

  if (master.get_digital(DIGITAL_L2))
    set_lift_state(MID);
}