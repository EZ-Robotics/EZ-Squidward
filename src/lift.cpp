#include "main.h"
#include "pros/adi.hpp"

using namespace ez;

PID liftPID{4, 0, 0, 0, "Lift"};
pros::Motor lift_motor(1, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);
int lift_max_speed = 127;
void set_lift_speed(int input) { lift_max_speed = abs(input); }
void set_lift(int input) { lift_motor = input; }
void reset_lift() { lift_motor.tare_position(); }

void set_lift_exit() { liftPID.set_exit_condition(80, 20, 300, 50, 500, 500); }

std::string lift_state_to_string(lift_state input) {
  switch (input) {
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
  current_lift_state = input;
  liftPID.set_target(input);
  set_lift_speed(input == UP || input == MID ? 127 : 100);
  std::cout << "New Lift State: " << lift_state_to_string(input) << "\n";
}

void liftTask() {
  double output = 0;
  long timer = 0;
  while (true) {
    double current = lift_motor.get_position();
    double clipped_pid = util::clip_num(liftPID.compute(current), lift_max_speed, -lift_max_speed);

    if (current_lift_state == DOWN) {
      if (current >= 20)
        output = clipped_pid;
      else {
        if (lift_motor.get_actual_velocity() == 0 && !pros::competition::is_disabled()) timer += util::DELAY_TIME;
        if (timer >= 250) {
          output = -3;
          reset_lift();
          timer = 250;
        } else
          output = -40;
      }
    } else {
      timer = 0;
      output = clipped_pid;
    }

    if (pros::competition::is_disabled()) timer = 0;

    set_lift(output);

    pros::delay(util::DELAY_TIME);
  }
}
pros::Task lift_task(liftTask);

void wait_lift() {
  while (liftPID.exit_condition(lift_motor, true) == ez::RUNNING) {
    pros::delay(ez::util::DELAY_TIME);
  }
}

bool l1_lock = 1;
void lift_control() {
  if (master.get_digital(DIGITAL_L1) && l1_lock == 1) {
    if (current_lift_state == UP || current_lift_state == MID)
      set_lift_state(DOWN);
    else
      set_lift_state(UP);
    l1_lock = 0;
  } else if (!master.get_digital(DIGITAL_L1) && l1_lock == 0) {
    l1_lock = 1;
  }

  if (master.get_digital(DIGITAL_R2))
    set_lift_state(MID);
}