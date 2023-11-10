#ifndef TIMER_INTERFACE_H
#define TIMER_INTERFACE_H

#include <ctime>

// code that uses these libraries for outputing a nicely formatted timer
// in the shell
// is from https://www.scaler.com/topics/cpp-timer/
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

bool timer_run = false;
int seconds = 0;
int minutes = 0;
// put whatever functions you think we'll need for interfacing here
// todo: pin numbers for each button
void increment_timer();
void set_timer();
void start_timer();
void stop_timer();
void display_time();

#endif