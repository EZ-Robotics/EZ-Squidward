/*
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once
#include "api.h"
#include <list>

void print_ez_template();

extern pros::Motor l_motor;
extern pros::Motor r_motor;

extern std::list<int> L_MOTOR_PORTS;
extern std::list<int> R_MOTOR_PORTS;

void chassis_motor_init();
void chassis_ports_init(std::list<int> l, std::list<int> r);

void set_l_chassis(int l);
void set_r_chassis(int r);
void set_tank(int l, int r);
void set_drive_brake(pros::motor_brake_mode_e_t input);

int right_sensor();
int right_velocity();

int left_sensor();
int left_velocity();

void reset_drive_sensor();

void tare_gyro();
float get_gyro();
bool imu_calibrate();

int sgn(int input);
float clip_num(float input, float max, float min);
