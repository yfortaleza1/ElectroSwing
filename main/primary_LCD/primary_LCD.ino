//Arduino code for LCD timer display and buttons
/*
* Project: Ava's Motorized Swing
* Author: Yoel Fortaleza, Jess Turner (source: `SheCodes` bot)
* Date:   Friday March 29, 2024
* Notes: 
Edited from original code via the source as of today.
Source 
https://www.youtube.com/watch?v=gBjHI8RJYGY
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Define button variables
const short int incrementPin = 10;
const short int decrementPin = 11;
const short int startPin = 12;
const short int stopPin = 13;
const short int buttonPins[] = {incrementPin,decrementPin,startPin,stopPin}; //used for buttons

//Define timer variable
int secTicks = 0;
int minTens = 0;
int minOnes = 0;
int secTens = 0;
int secOnes = 0;

const short int stopMotorPin = 8;
short int motorStatus = 1;


LiquidCrystal_I2C lcd(0x27, 20, 4); //Set LCD address for 16 chars and 2 line display (need to check address of COM port)

void setup(){
    lcd.init();
    lcd.backlight();
    attachInterrupt(digitalPinToInterrupt(incrementPin),incrementTime, HIGH); 
    attachInterrupt(digitalPinToInterrupt(decrementPin),countDown, CHANGE); 
    attachInterrupt(digitalPinToInterrupt(stopPin), sendStopSignal, CHANGE);
}

void loop(){
    
}


void incrementTime(){
      cli();
      if(minOnes < 9 && minOnes >= 0){
        minOnes +=1;//increment minsOnes place
      }

      else if(minOnes == 9 && minTens != 5){//ONES PLACE OVERFLOW CONDITION
        minOnes = 0;//set ones place of mines to 0
        minTens += 1;//incrmeent tens place of minutes
      }

      sei();
}

void countDown(){
    cli();
      if(digitalRead(stopPin) != LOW){//IF STOP BUTTON PRESSED, CLEAR TIMER AND END SWING
        //debounceButton(stopPin);
        clearTimer();//clear timer
      }

      else{//IF STOP BUTTON NOT PRESSED, DECREMENT NORMALLY

        if(secOnes != 0){
          secOnes = secOnes - 1;//decrement secOnes, a second should have passed
        }

        else if(secOnes == 0 && secTens != 0){

          secOnes = 9;
          secTens -=1;//decrement sec tens
        }

        else if(secOnes == 0 && secTens == 0 && minOnes != 0){

          minOnes -=1;
          secTens = 5;
          secOnes = 9;
        }

        else if(secOnes == 0 && secTens == 0 && minOnes == 0 && minTens != 0){

          minTens -=1;
          minOnes = 9;
          secTens = 5;
          secOnes = 9;
        }
      }

      sei();
}

void showTime(){
    lcd.setCursor(14,3);
    lcd.print(minTens);
    lcd.print(minOnes);
    lcd.print(":");
    lcd.print(secTens);
    lcd.print(secOnes);
}

void clearTimer(){
  secOnes = 0;
  secTens = 0;
  minOnes = 0;
  minTens = 0;
}

void sendStopSignal(){
    cli();
    clearTimer();
    motorStatus = 0;
    sei();
}