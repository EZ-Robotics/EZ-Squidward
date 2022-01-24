#include "main.h"


/////
// For instalattion, upgrading, documentations and tutorials, check out website!
// https://ez-robotics.github.io/EZ-Template/
/////


const int DRIVE_SPEED = 110; // This is 110/127 (around 87% of max speed).  We don't suggest making this 127.
                             // If this is 127 and the robot tries to heading correct, it's only correcting by
                             // making one side slower.  When this is 87%, it's correcting by making one side
                             // faster and one side slower, giving better heading correction.
const int TURN_SPEED  = 90;
const int SWING_SPEED = 90;



///
// Constants
///

// It's best practice to tune constants when the robot is empty and with heavier game objects, or with lifts up vs down.
// If the objects are light or the cog doesn't change much, then there isn't a concern here.

void default_constants() {
  chassis.set_slew_min_power(80, 80);
  chassis.set_slew_distance(7, 7);
  chassis.set_pid_constants(&chassis.headingPID, 11, 0, 20, 0);
  chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
  chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
}

void one_mogo_constants() {
  chassis.set_slew_min_power(80, 80);
  chassis.set_slew_distance(7, 7);
  chassis.set_pid_constants(&chassis.headingPID, 11, 0, 20, 0);
  chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
  chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
}

void two_mogo_constants() {
  chassis.set_slew_min_power(80, 80);
  chassis.set_slew_distance(7, 7);
  chassis.set_pid_constants(&chassis.headingPID, 11, 0, 20, 0);
  chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
  chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
}

void exit_condition_defaults() {
  chassis.set_exit_condition(chassis.turn_exit, 100, 3, 500, 7, 500, 500);
  chassis.set_exit_condition(chassis.swing_exit, 100, 3, 500, 7, 500, 500);
  chassis.set_exit_condition(chassis.drive_exit, 80, 50, 300, 150, 500, 500);
}

void modified_exit_condition() {
  chassis.set_exit_condition(chassis.turn_exit, 100, 3, 500, 7, 500, 500);
  chassis.set_exit_condition(chassis.swing_exit, 100, 3, 500, 7, 500, 500);
  chassis.set_exit_condition(chassis.drive_exit, 80, 50, 300, 150, 500, 500);
}

void tug (int attempts) {
  for (int i=0; i<attempts-1; i++) {
    // Attempt to drive backwards
    printf("i - %i", i);
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








void double_steal() {

  // Drive towards center goal
  claw_up();
  chassis.set_drive_pid(47, 127);
  // Slow down when close to center
  chassis.wait_until(40);
  chassis.set_max_speed(40);
  chassis.wait_until(43);
  claw_down();
  chassis.wait_drive();

  // Drive back with goal
  chassis.set_drive_pid(-38, 127, true);
  // Release goal during drive back
  chassis.wait_until(-14);

  // Check if interfered while driving back
  if (chassis.interfered) {
   tug(5);
   return;
 }

  chassis.set_max_speed(DRIVE_SPEED);
  claw_up();
  chassis.wait_drive();

  // Turn to face center goal
  chassis.set_turn_pid(-36, TURN_SPEED);
  chassis.wait_drive();

  // Drive towards center goal
  chassis.set_drive_pid(53, 127);
  set_lift_state(DOWN);
  chassis.wait_until(43);
  chassis.set_max_speed(30);
  chassis.wait_until(48);
  claw_down();
  chassis.wait_drive();

  // Drive back with goal
  chassis.set_drive_pid(-38, 127, true);
  // Release goal during drive back
  chassis.wait_until(-27);

  // Check if interfered while driving back
  if (chassis.interfered) {
   tug(5);
   return;
 }

  chassis.set_max_speed(60);
  //lift_lil_down(true);       ////////////////////
  // claw_up();
  chassis.wait_drive();
  pros::delay(250);

  // Turn to face alliance goal
  chassis.set_turn_pid(-90, TURN_SPEED);
  set_mogo_state(m::DOWN);
  chassis.wait_drive();

  // Drive into goal
  chassis.set_drive_pid(-19, 65, true);
  chassis.wait_drive();
  set_mogo_state(m::UP);
  wait_mogo();

  // Turn to corner
  chassis.set_swing_pid(ez::LEFT_SWING, -180, SWING_SPEED);
  chassis.wait_drive();

  // Drive into corner
  chassis.set_drive_pid(17, DRIVE_SPEED, true);
  chassis.wait_drive();

  // Release goal
  pros::delay(100);
  claw_up();
  pros::delay(500);

  // Drive away
  chassis.set_drive_pid(-27, DRIVE_SPEED, true);
  chassis.wait_drive();

  chassis.set_turn_pid(-360, 80);
  chassis.wait_drive();

}



void steal_one() {

  // Drive towards center goal
  claw_up();
  chassis.set_drive_pid(45, 127);
  set_lift_state(DOWN);
  // Slow down when close to center
  chassis.wait_until(35);
  chassis.set_max_speed(40);
  chassis.wait_drive();

  // Grab goal
  claw_down();

  // Turn 180
  chassis.set_turn_pid(-180, TURN_SPEED);
  chassis.wait_drive();

  // Drive back with goal
  chassis.set_drive_pid(27, 127, true);
  chassis.wait_until(20);
  chassis.set_max_speed(DRIVE_SPEED);
  set_mogo_state(m::DOWN);
  chassis.wait_drive();

  // Turn to face alliance goal
  chassis.set_turn_pid(-90, TURN_SPEED);
  chassis.wait_drive();

  // Drive into alliance goal
  chassis.set_drive_pid(-14, 50, true);
  chassis.wait_drive();
  set_mogo_state(m::UP);
  wait_mogo();

  // Swing to corner
  chassis.set_swing_pid(ez::LEFT_SWING, -180, TURN_SPEED);
  chassis.wait_drive();

  // Drive into corner
  chassis.set_drive_pid(17, DRIVE_SPEED, true);
  chassis.wait_drive();

  // Release goal
  pros::delay(100);
  claw_up();
  pros::delay(500);

  // Drive away
  chassis.set_drive_pid(-27, DRIVE_SPEED, true);
  chassis.wait_drive();

  // Turn to face opponents
  chassis.set_turn_pid(-360, TURN_SPEED);
  chassis.wait_drive();

}



void plat_down_center_hit() {

  // Tunr to face center
  chassis.set_drive_pid(12, TURN_SPEED);
  chassis.wait_drive();

  // Drive into goal
  chassis.set_drive_pid(56, 127);
  chassis.wait_until(42);
  claw_down();
  chassis.set_max_speed(DRIVE_SPEED);
  chassis.wait_drive();

  // Swing to face centerline
  chassis.set_swing_pid(ez::LEFT_SWING, 90, TURN_SPEED);
  chassis.wait_drive();


  // FULL SEND
  chassis.set_drive_pid(62, DRIVE_SPEED, true);
  chassis.wait_drive();

  // back up a little to block :)
  chassis.set_drive_pid(-12, DRIVE_SPEED);
  chassis.wait_drive();

}



void plat_down_center() {

  // Drive towards center goal
  claw_up();
  chassis.set_drive_pid(47, 127);
  // Slow down when close to center
  chassis.wait_until(40);
  chassis.set_max_speed(40);
  chassis.wait_until(43);
  claw_down();
  chassis.wait_drive();

  // Drive back with goal
  chassis.set_drive_pid(-38, 127, true);
  // Release goal during drive back
  chassis.wait_until(-14);

  // Check if interfered while driving back
  if (chassis.interfered) {
   tug(5);
   return;
 }

  chassis.set_max_speed(DRIVE_SPEED);
  chassis.wait_drive();

  // Turn to face alliance oal
  chassis.set_turn_pid(-48, TURN_SPEED);
  chassis.wait_drive();

  set_mogo_state(m::DOWN);
  wait_mogo();

  // Drive into alliance goal
  chassis.set_drive_pid(-11, 80);
  chassis.wait_drive();

  set_mogo_state(m::UP);
  wait_mogo();

  // Drive forward
  chassis.set_drive_pid(20, DRIVE_SPEED, true);
  chassis.wait_drive();


  // Turn to face opponents home zone
  chassis.set_turn_pid(10, TURN_SPEED);
  chassis.wait_drive();

  /*

  // Drive as far as possible with alliance goal still scored
  chassis.set_drive_pid(14, DRIVE_SPEED, true);
  chassis.wait_drive();

  */

}



void dislike_you_lucas() {

  chassis.set_drive_pid(58, 127);
  chassis.wait_until(40);
  chassis.set_max_speed(40);
  chassis.wait_until(56);
  claw_down();
  chassis.wait_drive();

  //set_drive_pid(drive, -24, 127);
  chassis.set_swing_pid(ez::RIGHT_SWING, 152.5, 127);
  chassis.wait_until(45);
  chassis.set_max_speed(TURN_SPEED);
  set_mogo_state(m::DOWN);
  wait_mogo();
  chassis.wait_drive();

  chassis.set_drive_pid(-26, 80);
  chassis.wait_drive();

  set_mogo_state(m::UP);
  wait_mogo();

  chassis.set_drive_pid(30, DRIVE_SPEED, true);
  chassis.wait_drive();

  chassis.set_turn_pid(147, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(55, DRIVE_SPEED, true);
  chassis.wait_drive();

  pros::delay(250);
  claw_up();
  pros::delay(250);

  chassis.set_drive_pid(-10, DRIVE_SPEED, true);
  chassis.wait_drive();

  chassis.set_turn_pid(22.5 + 7, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(33, DRIVE_SPEED, true);
  set_mogo_state(m::DOWN);
  wait_mogo();
  chassis.wait_until(28);
  chassis.set_max_speed(40);
  claw_down();
  chassis.wait_drive();

  chassis.set_swing_pid(ez::LEFT_SWING, 22.5 + 7 - 45, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(-18, DRIVE_SPEED, true);
  chassis.wait_drive();

  chassis.set_swing_pid(ez::RIGHT_SWING, 22.5 + 7, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(-3, DRIVE_SPEED);
  chassis.wait_drive();

  claw_up();


}