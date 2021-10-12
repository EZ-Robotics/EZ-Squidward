#include "main.h"

const int MOGO_DOWN = 80;
const int MOGO_UP   = 530;
const int MOGO_MID  = 350;

const int MAX_SPEED = 200;

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

void zero_mogo()    { mogo.tare_position(); }
int  get_mogo()     { return mogo.get_position(); }
int  get_mogo_vel() { return mogo.get_actual_velocity(); }

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
mogo_down (bool hold) {
  set_mogo_position(MOGO_DOWN, MAX_SPEED);
  m_is_in = timeout(MOGO_DOWN, get_mogo_vel(), get_mogo());

  if (hold) {
    b_mogo_up = true;
    m_is_at_down = false;
    pros::delay(DELAY_TIME);
    mogo_down(!m_is_in);
  }
}

void
mogo_up(bool hold) {
  set_mogo_position(MOGO_MID, MAX_SPEED);
  m_is_down = timeout(MOGO_MID, get_mogo_vel(), get_mogo());

  if (hold) {
    m_is_at_down = true;
    pros::delay(DELAY_TIME);
    mogo_up(!m_is_down);
  }
}

void
mogo_mid(bool hold) {
  set_mogo_position(MOGO_UP, MAX_SPEED);
  m_is_mid = timeout(MOGO_UP, get_mogo_vel(), get_mogo());

  if (hold) {
    b_mogo_up = false;
    m_is_at_down = false;
    pros::delay(DELAY_TIME);
    mogo_mid(!m_is_mid);
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
    mogo_up(false);
  else if (b_mogo_up)
    mogo_down(false);
  else if (!b_mogo_up)
    mogo_mid(false);
}
