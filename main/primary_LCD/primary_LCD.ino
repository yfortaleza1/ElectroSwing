//Arduino code for LCD timer display and buttons
/*
* Project: Ava's Motorized Swing
* Author: Yoel Fortaleza (source: `SheCodes` bot)
* Date:   Friday March 29, 2024
* Notes: 
Edited from original code via the source as of today.
Source 
https://www.youtube.com/watch?v=gBjHI8RJYGY
*/



#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Define variables
int currentTimeValue[4];
int currentState = 0; //0 - timer is reset to 0, 1 - anything else
int seconds = 0;
int secondsTens = 0;
int minutes = 0;
int hours = 0;


LiquidCrystal_I2C lcd(0x27, 20, 4); //Set LCD address for 16 chars and 2 line display (need to check address of COM port)

void setup(){
    lcd.init();
    lcd.backlight();
}

void loop(){
    lcd.setCursor(1,0);
    lcd.print("Hello World!");
    delay(3000);
    lcd.clear();

    lcd.setCursor(2,0);
    lcd.print("This is an LCD");
    lcd.setCursor(2,1);
    lcd.print("screen test");
    delay(3000);
    lcd.clear();

}


void incrementTime(){

}

void countDown(){

}

void showTime(){
    lcd.setCursor(14,3);
    lcd.print(currentTimeValue[0]);
    lcd.print(currentTimeValue[1]);
    lcd.print(":");
    lcd.print(currentTimeValue[2]);
    lcd.print(currentTimeValue[3]);
}