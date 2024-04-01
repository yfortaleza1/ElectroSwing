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
#define SECONDS_PER_MINUTE 60

#define MAX_TENS_DIGIT 5
#define MAX_ONES_DIGIT 9

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h> // floor function.

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


int calculateTotalSeconds() {
    // time in seconds!

    return (secOnes + 10 * secTens + // add up the seconds
            // add up the minutes
            SECONDS_PER_MINUTE * minOnes + 
            (SECONDS_PER_MINUTE*10) * secTens)
}

void updateDigitsFromTotalSeconds(int totalSeconds) {
    /*
    Change our variables which correspond to each digit we display for the time
    based on the given number of seconds.
    */
    int totalMinutes = floor(totalSeconds / SECONDS_PER_MINUTE);
    int leftoverSeconds = totalSeconds % SECONDS_PER_MINUTE;

    // reassign our global variales for the digits
    // example 300 seconds should be 05:00
    minTens = floor(totalMinutes / MAX_TENS_DIGIT + 1) ;
    minOnes = totalMinutes % (MAX_ONES_DIGIT + 1);
    secTens = floor(leftoverSeconds / MAX_TENS_DIGIT + 1) ;
    secOnes = leftoverSeconds % (MAX_ONES_DIGIT + 1);

}

// Author: Jess
// 
void incrementTimer(){
    cli();

    int nextTotalSeconds = calculateTotalSeconds();
    nextTotalSeconds += SECONDS_INCREMENT;

    updateDigitsFromTotalSeconds(nextTotalSeconds);

    sei();
}

// Author: Marc (see ../primary/primary.ino)
//this function is responsible for time decrementing logic
//Function decrements timer by 1 SECOND
//THIS FUNCTION WILL BE CALLED BY ISR EVERY SECOND TO GET THE TIMING RIGHT
void decrementTimer(){
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