//Arduino code for LCD timer display and buttons
/*
* Project: Ava's Motorized Swing
* Author: Yoel Fortaleza, Jess Turner
* Date:   Friday March 29, 2024 - Monday April 1, 2024
* Notes: 
Edited from original code via the source as of today.
Source 
https://www.youtube.com/watch?v=gBjHI8RJYGY
*/

#define SECONDS_INCREMENT 30

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Define variables
short int secTicks = 0;//used to keep track when a second should occur, clock is 16MHZ, so count up to 16 mill and decremenet timer
short int minTens = 0;//tens place for minutes
short int minOnes = 0;//ones place for minutes
short int secTens = 0;//tens place for seconds
short int secOnes = 0;//ones place for seconds
int currentState = 0; //0 - timer is reset to 0, 1 - anything else


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
    lcd.print(minTens);
    lcd.print(minOnes);
    lcd.print(":");
    lcd.print(secTens);
    lcd.print(secOnes);
}