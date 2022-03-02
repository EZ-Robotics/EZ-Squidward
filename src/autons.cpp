#include "main.h"

// Speeds used throughout the code
const int DRIVE_SPEED = 110;  // This is 110/127 (around 87% of max speed).  We don't suggest making this 127.
                              // If this is 127 and the robot tries to heading correct, it's only correcting by
                              // making one side slower.  When this is 87%, it's correcting by making one side
                              // faster and one side slower, giving better heading correction.
const int TURN_SPEED = 90;
const int SWING_SPEED = 90;

// Default constants 
void default_constants() {
  chassis.set_exit_condition(chassis.turn_exit, 100, 3, 500, 7, 500, 500);
  chassis.set_exit_condition(chassis.swing_exit, 100, 3, 500, 7, 500, 500);
  chassis.set_exit_condition(chassis.drive_exit, 80, 50, 300, 150, 500, 500);

  chassis.set_slew_min_power(80, 80);
  chassis.set_slew_distance(7, 7);
  chassis.set_pid_constants(&chassis.headingPID, 11, 0, 20, 0);
  chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
  chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
}

// Tug of war 
void tug(int attempts) {
  for (int i = 0; i < attempts - 1; i++) {
    // Attempt to drive backwards
    chassis.set_drive_pid(-12, 127);
    chassis.wait_drive();

    // If failsafed...
    if (chassis.interfered) {
      chassis.reset_drive_sensor();
      chassis.set_drive_pid(-2, 20);
      pros::delay(1000);
    }
    // If robot successfully drove back, return
    else {
      return;
    }
  }
}



// Steal 2 neutral goals, aim for center, starts under plat up
void double_steal() {
  // Drive towards center goal
  chassis.set_drive_pid(60, 127);
  chassis.wait_until(45);
  claw_down();

  // Drive back with goal
  chassis.set_drive_pid(-22, 127);
  chassis.wait_until(-10);
  chassis.set_max_speed(DRIVE_SPEED);
  chassis.wait_drive();

  if (chassis.interfered) {
    tug(5);
    return;
  }

  chassis.set_turn_pid(-120, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(-29, 127);
  chassis.wait_until(-24);
  chassis.set_max_speed(50);
  chassis.wait_drive();

  if (chassis.interfered) {
    tug(5);
    return;
  }

  set_dougie_state(2);
  pros::delay(100);

  chassis.set_swing_pid(ez::RIGHT_SWING, -190, SWING_SPEED);
  pros::delay(100);
  set_dougie_state(3);
  chassis.wait_drive();

  if (chassis.interfered) {
    tug(5);
    return;
  }

  chassis.set_drive_pid(37, DRIVE_SPEED, true);
  chassis.wait_drive();
  pros::delay(200);
  claw_up();
  pros::delay(200);

  chassis.set_drive_pid(-14.5, DRIVE_SPEED, true);
  chassis.wait_drive();

  chassis.set_turn_pid(-252, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(10, 70, true);
  chassis.wait_drive();
  claw_down();

  chassis.set_turn_pid(-342, TURN_SPEED);
  chassis.wait_drive();
}


// Steals one neutral goal from plat up
void steal_one() {
  // Drive towards center goal
  chassis.set_drive_pid(60, 127);
  chassis.wait_until(38);
  claw_down();

  // Drive back with goal
  chassis.set_drive_pid(-26.5, 127);
  chassis.wait_until(-10);
  chassis.set_max_speed(DRIVE_SPEED);
  chassis.wait_drive();

  if (chassis.interfered) {
    tug(5);
    return;
  }

  // chassis.set_swing_pid(ez::LEFT_SWING, -90, SWING_SPEED);
  chassis.set_turn_pid(-90, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(-15, 60);
  chassis.wait_drive();

  pros::delay(100);
  set_dougie_state(2);
  pros::delay(100);

  chassis.set_turn_pid(0, TURN_SPEED);
  set_dougie_state(3);
  chassis.wait_drive();
}



// Steals one neutral goal from plat down
void plat_down_center() {
  // Drive towards center goal
  chassis.set_drive_pid(60, 127);
  chassis.wait_until(40);
  claw_down();

  // Drive back with goal
  chassis.set_drive_pid(-33, 127);
  chassis.wait_until(-20);
  chassis.set_max_speed(DRIVE_SPEED);
  chassis.wait_drive();

  if (chassis.interfered) {
    tug(5);
    return;
  }

  chassis.set_swing_pid(ez::LEFT_SWING, -55, SWING_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(-6, 60);
  chassis.wait_drive();

  pros::delay(100);
  set_dougie_state(2);
  pros::delay(100);

  chassis.set_drive_pid(12, DRIVE_SPEED, true);
  set_dougie_state(3);
  chassis.wait_drive();

  chassis.set_turn_pid(-10, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(12, DRIVE_SPEED, true);
  chassis.wait_drive();
}