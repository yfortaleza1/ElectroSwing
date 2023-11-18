/*
* File: main.ino
* Project: Ava's Motorized Swing
* Authors: Yoel Fortaleza, Alec Kalisch, Marc Conn, Jess Turner
* Date:    10/28/2023 - 11/17/2023
* Notes: 
- This file will be where we call all the modules of our project
-> [X] motor
-> [ ] timer
-> etc.
*/
#include "motorRotator.h"
// todo @LT69018 incorporate timer code into a .ino and use LCD display

void setup(){
    motorRotatorSetup();
}

void loop(){
    motorRotatorLoop();
}