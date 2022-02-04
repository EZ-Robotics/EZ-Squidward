#include "main.h"
#include "pros/misc.hpp"

using namespace ez;
PID mogoPID{3, 0, 0, 0, "Mogo"};
pros::Motor mogo_motor(10, MOTOR_GEARSET_36, true, MOTOR_ENCODER_DEGREES);
int mogo_max_speed = 127;
void set_mogo_speed(int input) { mogo_max_speed = abs(input); }
void set_mogo(int input) { mogo_motor = input; }
void reset_mogo() { mogo_motor.tare_position(); }

void set_mogo_exit() { mogoPID.set_exit_condition(80, 20, 300, 50, 500, 500); }

std::string mogo_state_to_string(m::mogo_state input) {
  switch (input) {
    case m::DOWN:
      return "Down";
      break;
    case m::UP:
      return "Up";
      break;
    default:
      return "Out of bounds lift state";
      break;
  }
}

m::mogo_state current_mogo_state;
void set_mogo_state(m::mogo_state input) {
  current_mogo_state = input;
  mogoPID.set_target(input);
  std::cout << "\nNew Mogo State: " << mogo_state_to_string(input);
}

void zero_mogo() {
  if (pros::competition::is_disabled()) return;

  set_mogo(-60);
  int timer = 0;
  while (true) {
    if (mogo_motor.get_actual_velocity() == 0) timer+=util::DELAY_TIME;
    if (timer >= 250) 
      break;

    pros::delay(util::DELAY_TIME);
  }
  set_mogo(0);
  reset_mogo();
}

void mogoTask() {
  double output = 0;
  long timer = 0;
  zero_mogo();
  while (true) {
    double current = mogo_motor.get_position();
    double clipped_pid = util::clip_num(mogoPID.compute(current), mogo_max_speed, -mogo_max_speed);

    if (current_mogo_state == m::DOWN) {
      if (current < m::DOWN - 40)
        output = clipped_pid;
      else {
        if (mogo_motor.get_actual_velocity() == 0 && !pros::competition::is_disabled()) timer += util::DELAY_TIME;
        if (timer >= 250) {
          output = 5;
          timer = 250;
        }
      }
    } else {
      timer = 0;
      output = clipped_pid;
    }

    if (pros::competition::is_disabled()) timer = 0;

    set_mogo(output);

    pros::delay(util::DELAY_TIME);
  }
}
pros::Task mogo_task(mogoTask);

void wait_mogo() {
  while (mogoPID.exit_condition(mogo_motor, true) == ez::RUNNING) {
    pros::delay(ez::util::DELAY_TIME);
  }
}

bool last_r1 = 0;
void mogo_control() {
  if (master.get_digital(DIGITAL_R1) && last_r1 == 0) {
    if (current_mogo_state == m::UP)
      set_mogo_state(m::DOWN);
    else
      set_mogo_state(m::UP);
  } 
  last_r1 = master.get_digital(DIGITAL_R1);
}