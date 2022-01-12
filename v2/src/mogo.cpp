#include "main.h"

using namespace ez;

PID mogoPID{3, 0, 0, 0, "Mogo"};
pros::Motor mogo_motor(10, MOTOR_GEARSET_36, true, MOTOR_ENCODER_DEGREES);
int mogo_max_speed = 127;
void set_mogo_speed(int input) { mogo_max_speed = abs(input); }
void set_mogo(int input) { mogo_motor = input; }
void reset_mogo() { mogo_motor.tare_position(); }

void set_mogo_exit() { mogoPID.set_exit_condition(80, 20, 300, 50, 500, 500); }

m::mogo_state current_mogo_state;
void set_mogo_state(m::mogo_state input) {
  current_mogo_state = input;
  mogoPID.set_target(input);
  printf("%i\n", input);
}

void mogoTask() {
  double output = 0;
  while (true) {
    double current = mogo_motor.get_position();
    double clipped_pid = util::clip_num(mogoPID.compute(current), mogo_max_speed, -mogo_max_speed);

    output = clipped_pid;

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

bool r1_lock = 1;
void mogo_control() {
  if (master.get_digital(DIGITAL_R1) && r1_lock == 1) {
    if (current_mogo_state == m::UP)
      set_mogo_state(m::DOWN);
    else
      set_mogo_state(m::UP);
    r1_lock = 0;
  } else if (!master.get_digital(DIGITAL_R1) && r1_lock == 0) {
    r1_lock = 1;
  }
}