//Arduino code for LCD timer display and buttons
/*
* Project: Ava's Motorized Swing
* Author: Yoel Fortaleza (source: `SheCodes` bot)
* Date:   Friday March 29, 2024
* Notes: 
Edited from original code via the source as of today.
Source 
https://www.shecodes.io/athena/38307-how-to-create-a-timer-with-lcd-display-using-arduino
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Define variables
int currentState = 1;
int seconds = 0;
int minutes = 0;
int hours = 0;


LiquidCrystal_I2C lcd(0x27, 20, 4) //Set LCD address for 16 chars and 2 line display (need to check address of COM port)

void setup(){

}

void loop(){

}


void incrementTime(){

}

void countDown(){

}