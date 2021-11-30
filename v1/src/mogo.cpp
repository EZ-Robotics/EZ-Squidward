#include "main.h"

const int MOGO_DOWN = 80;
const int MOGO_UP   = 530;
const int MOGO_MID  = 350;

const int MAX_SPEED = 200;

// Lift Down Constants
const int DOWN_HARD     = 50; // Power to bring the lift down fast
const int DOWN_SMALL    = 2;  // Power to hold the lift down
const int STOP_PID      = MOGO_UP-20;  // When lift is below this, it will turn off PID and move to velocity logic
const int VEL_TIMEOUT   = 100; // When velocity is 0 for this long, the lift is down

// Driver Control Parameters
bool b_mogo_up = true;
bool m_is_at_down = false;
int mogo_lock   = 0;
int mogo2_lock   = 0;
int controller_mogo_timer = 0;

bool m_is_in = false;
bool m_is_mid = false;
bool m_is_down = false;


pros::Motor mogo(10, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);


void set_mogo(int input)  { mogo = input; }

void zero_mogo(int input) { mogo.set_zero_position(input); }
int  get_mogo()           { return mogo.get_position(); }
int  get_mogo_vel()       { return mogo.get_actual_velocity(); }

void
set_mogo_position(int target, int speed) {
  mogo.move_absolute(target, speed);
}


///
// Tilter Control
//  - when L2 is pressed, toggle between in and out
//  - when L2 is held bring mogo to down
///

void
mogo_up (bool hold) {
  set_mogo_position(MOGO_DOWN, MAX_SPEED);
  m_is_in = timeout(MOGO_DOWN, get_mogo_vel(), get_mogo());

  if (hold) {
    b_mogo_up = true;
    m_is_at_down = false;
    pros::delay(DELAY_TIME);
    mogo_up(!m_is_in);
  }
}

void
mogo_mid(bool hold) {
  set_mogo_position(MOGO_MID, MAX_SPEED);
  m_is_down = timeout(MOGO_MID, get_mogo_vel(), get_mogo());

  if (hold) {
    m_is_at_down = true;
    pros::delay(DELAY_TIME);
    mogo_mid(!m_is_down);
  }
}

void
mogo_down (bool hold) {
  static int t, x;
  static bool stop;

  // If the current position is greater then some number, run the built in PID
  if (get_mogo() < STOP_PID) {
    printf("pid - ");
    set_mogo_position(MOGO_UP, MAX_SPEED);
    //is_in = timeout(LIFT_DOWN, get_lift_vel(), get_lift());
    m_is_mid = false;
    stop = false;
  }
  // When the robot is within 50,
  else {
    // When the velocity is 0, reset the sensor and set the lift a little down
    if (get_mogo_vel() == 0) {
      printf("vel 0 - ");
      t+=DELAY_TIME;
      if (t >= VEL_TIMEOUT) {
        zero_mogo(MOGO_UP);
        set_mogo(DOWN_SMALL);
        m_is_mid = true;
        stop = true;
        t = 0;
      }
    }
    // When the velocity isn't 0, bring the lift down at some power
    else {
      printf("vel not 0 - ");
      m_is_mid = stop?true:false;
      t = 0;
      set_mogo(stop?DOWN_SMALL:DOWN_HARD);
    }
  }

  printf("%i\n", m_is_mid);


  // Recursive for autonomous
  if (hold) {
    // Variables for driver control
    b_mogo_up = false;
    m_is_at_down = false;
    pros::delay(DELAY_TIME);
    mogo_down(!m_is_mid);
  }
}


///
// Driver Control
///
void
mogo_control() {
  if (master.get_digital(DIGITAL_R1) && mogo_lock==0) {
    if (m_is_at_down)
      b_mogo_up = false;
    else
      b_mogo_up = !b_mogo_up;

    m_is_at_down = false;
    mogo_lock = 1;
  }
  else if (master.get_digital(DIGITAL_R1)) {
    controller_mogo_timer+=DELAY_TIME;
    if (controller_mogo_timer>=250)
      m_is_at_down = true;
  }
  else if (!master.get_digital(DIGITAL_R1)) {
    mogo_lock  = 0;
    controller_mogo_timer = 0;
  }

  if (master.get_digital(DIGITAL_R2) && mogo2_lock==0) {
    mogo2_lock = 1;
    if (m_is_at_down) {
      b_mogo_up = false;
      m_is_at_down = false;
    } else if (b_mogo_up) {
      b_mogo_up = false;
    } else {
      m_is_at_down = true;
    }
  }
  else if (!master.get_digital(DIGITAL_R2)) {
    mogo2_lock = 0;
  }

  if (m_is_at_down)
    mogo_mid(false);
  else if (b_mogo_up)
    mogo_up(false);
  else if (!b_mogo_up)
    mogo_down(false);
}
