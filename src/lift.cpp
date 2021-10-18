#include "main.h"

const int LIFT_DOWN = 0;
const int LIFT_LIL_DOWN = 100;
const int LIFT_DOWN_BIG = 200;
const int LIFT_UP   = 850;
const int LIFT_MID  = 700;

const int MAX_SPEED = 200; //rpm

// Lift Down Constants
const int DOWN_HARD     = -50; // Power to bring the lift down fast
const int DOWN_SMALL    = -2;  // Power to hold the lift down
const int STOP_PID      = 20;  // When lift is below this, it will turn off PID and move to velocity logic
const int VEL_TIMEOUT   = 100; // When velocity is 0 for this long, the lift is down
const int FIRST_TIMEOUT = 100; // Set the lift to a little bit of power at the start of autonomous

// Driver Control Parameters
bool b_lift_up = true;
bool is_at_down = false;
int lift_lock   = 0;
int controller_lift_timer = 0;

bool is_in = false;
bool is_lil_in = false;
bool is_mid = false;
bool is_down = false;
bool is_down_in = false;


pros::Motor lift(1, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);


void set_lift(int input)  { lift = input; }

void zero_lift()     { lift.tare_position(); }
int  get_lift()     { return lift.get_position(); }
int  get_lift_vel() { return lift.get_actual_velocity(); }

void
set_lift_position(int target, int speed) {
  lift.move_absolute(target, speed);
}


///
// Tilter Control
//  - when L2 is pressed, toggle between in and out
//  - when L2 is held bring lift to down
///
bool timeout(int target, int vel, int current) {
  static int vel_timeout;
  static int small_timeout;

  if (abs(target-current) < 20) {
    small_timeout+=DELAY_TIME;
    if (small_timeout>50) {
      small_timeout = 0;
      vel_timeout = 0;
      return true;
    }
  } else {
    small_timeout = 0;
  }

  if (vel == 0) {
    vel_timeout+=DELAY_TIME;
    if (vel_timeout>250) {
      vel_timeout = 0;
      small_timeout = 0;
      return true;
    }
  } else {
    vel_timeout = 0;
  }

  return false;
}

void
lift_down (bool hold) {
  //printf("lift down\n");

  static int t, x;
  static bool stop, first_run = true;

  // When run during autonomous, bring the lift down at power to make sure it's all the way down
  if (hold && first_run) {
    set_lift(DOWN_HARD);
    x+=DELAY_TIME;
    if (x>=FIRST_TIMEOUT) {
      first_run = false;
      x = 0;
    }
  }


  // If the current position is greater then some number, run the built in PID
  if (get_lift() > STOP_PID) {
    set_lift_position(LIFT_DOWN, MAX_SPEED);
    //is_in = timeout(LIFT_DOWN, get_lift_vel(), get_lift());
    is_in = false;
    stop = false;
  }
  // When the robot is within 50,
  else {
    // When the velocity is 0, reset the sensor and set the lift a little down
    if (get_lift_vel() == 0) {
      t+=DELAY_TIME;
      if (t >= VEL_TIMEOUT) {
        zero_lift();
        set_lift(DOWN_SMALL);
        is_in = true;
        stop = true;
        t = 0;
      }
    }
    // When the velocity isn't 0, bring the lift down at some power
    else {
      is_in = stop?true:false;
      t = 0;
      set_lift(stop?DOWN_SMALL:DOWN_HARD);
    }
  }


  // Recursive for autonomous
  if (hold) {
    // Variables for driver control
    b_lift_up = true;
    is_at_down = false;
    pros::delay(DELAY_TIME);
    first_run = x>=FIRST_TIMEOUT?is_in:first_run;
    lift_down(!is_in);
  }
}

void
lift_lil_down(bool hold) {
  //printf("lift lil down\n");
  set_lift_position(LIFT_LIL_DOWN, MAX_SPEED);
  is_lil_in = timeout(LIFT_LIL_DOWN, get_lift_vel(), get_lift());

  if (hold) {
    b_lift_up = true;
    is_at_down = false;
    pros::delay(DELAY_TIME);
    lift_lil_down(!is_lil_in);
  }
}


void
lift_down_big (bool hold) {
  //printf("lift down big\n");
  set_lift_position(LIFT_DOWN_BIG, MAX_SPEED);
  is_down_in = timeout(LIFT_DOWN_BIG, get_lift_vel(), get_lift());

  if (hold) {
    b_lift_up = true;
    is_at_down = false;
    pros::delay(DELAY_TIME);
    lift_down_big(!is_down_in);
  }
}

void
lift_up(bool hold) {
  //printf("lift up\n");
  set_lift_position(LIFT_UP, MAX_SPEED);
  is_down = timeout(LIFT_UP, get_lift_vel(), get_lift());

  if (hold) {
    is_at_down = true;
    pros::delay(DELAY_TIME);
    lift_up(!is_down);
  }
}

void
lift_mid(bool hold) {
  //printf("lift mid\n");
  set_lift_position(LIFT_MID, MAX_SPEED);
  is_mid = timeout(LIFT_MID, get_lift_vel(), get_lift());

  if (hold) {
    b_lift_up = false;
    is_at_down = false;
    pros::delay(DELAY_TIME);
    lift_mid(!is_mid);
  }
}

bool start_timer = false;
bool done_delaying = false;
int timer_lock = 0;
int timer = 0;
const int DELAY_AFTER_CLOSE = 100;
///
// Driver Control
///
void
lift_control() {
  if (master.get_digital(DIGITAL_L1) && lift_lock==0) {
    if (is_at_down)
      b_lift_up = true;
    else
      b_lift_up = !b_lift_up;

    is_at_down = false;
    lift_lock = 1;
  }
  else if (master.get_digital(DIGITAL_L1)) {
    controller_lift_timer+=DELAY_TIME;
    if (controller_lift_timer>=300)
      is_at_down = true;
  }
  else if (!master.get_digital(DIGITAL_L1)) {
    lift_lock  = 0;
    controller_lift_timer = 0;
  }


  if (master.get_digital(DIGITAL_L2) && timer_lock == 0 && !done_delaying) {
    start_timer = true;
    timer_lock = 1;
  }
  else if (master.get_digital(DIGITAL_L2) && timer_lock == 0 && done_delaying) {
    done_delaying = false;
    timer_lock = 1;
  }
  else if (!master.get_digital(DIGITAL_L2)) {
    timer_lock = 0;
  }

  if (start_timer) {
    timer+=DELAY_TIME;
    if (timer >= DELAY_AFTER_CLOSE) {
      timer = 0;
      start_timer = false;
      done_delaying = true;
    }
  }

  //if (!claw_toggle) done_delaying = false;

  // Lift all the way up
  if (is_at_down)
    lift_up(false);

  // Lift down
  else if (b_lift_up) {
    // When the claw is closed,
    if (done_delaying) {
      lift_lil_down(false);
    }
    // When the claw is open,
    else {
      lift_down(false);
    }
  }
  // Lift almost all the way up
  else if (!b_lift_up)
    lift_mid(false);
}
