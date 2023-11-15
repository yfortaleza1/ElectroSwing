// Date: Tues Nov. 14
// source: https://www.shecodes.io/athena/38307-how-to-create-a-timer-with-lcd-display-using-arduino

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int seconds = 0;
int minutes = 0;
int hours = 0;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Time: 00:00:00");
}

void loop() {
  // increment the seconds
  seconds++;
  if (seconds > 59) {
    minutes++;
    seconds = 0;
  }
  if (minutes > 59) {
    hours++;
    minutes = 0;
  }
  if (hours > 23) {
    hours = 0;
  }

  // create a string for the current time
  String currentTime = "";
  if (hours < 10) {
    currentTime += "0";
  }
  currentTime += hours;
  currentTime += ":";
  if (minutes < 10) {
    currentTime += "0";
  }
  currentTime += minutes;
  currentTime += ":";
  if (seconds < 10) {
    currentTime += "0";
  }
  currentTime += seconds;

  // Print the time to the LCD
  lcd.setCursor(0, 1);
  lcd.print(currentTime);

  // wait for one second
  delay(1000);
}