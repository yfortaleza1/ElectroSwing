/*
Name: primary.ino
Authors: Marc Conn
(contributers - Yoel Fortaleza, Jess Turner)
Date: 3/27-4/3 (planning on changing this current code to be LCD display rather than seven segment)
Description: 
This file will control the timing for the swing system.
Todo: use master pin to signal to secondary that it can do its logic. 
(Meaning that ride time is not zero, so it should continue swinging.)
*/

//THIS IS USING Arduino UNO R3
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <math.h> // floor function.
#include <TimerOne.h>

//Define button variables
const short int incrementPin = 11;
const short int decrementPin = 10;
const short int startPin = 2; //Interrupt capable pin
const short int stopPin = 3; //Interrupt capable pin
const short int masterPin = 6;
const short int buttonPins[] = {incrementPin,decrementPin,startPin,stopPin};//used for buttons

//Define global timer variables
volatile int minutes = 0;
volatile int seconds = 0;
volatile bool countDownActive = false;

// Global variables for debounce
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Variables to store the button states (this is for reading debounces)
bool lastIncrementButtonState = HIGH;
bool lastDecrementButtonState = HIGH;
bool lastStartButtonState = HIGH;
bool lastStopButtonState = HIGH;

//Initialize LCD
LiquidCrystal_I2C lcd(0x27, 20, 4); //Set LCD address for 16 chars and 2 line display (need to check address of COM port)

//Function declarations
void buttonSetup();
void incrementTime();
void decrementTime();
void startTime();
void stopTime();
void updateTime();
void timerTick();

void setup()
{
  //USED FOR TESTING
  buttonSetup();
  clearTimer();
  updateTime();
  
  lcd.init();
  lcd.backlight();

  attachInterrupt(digitalPinToInterrupt(stopPin), stopTime, CHANGE);
  attachInterrupt(digitalPinToInterrupt(startPin), startTime, CHANGE);

  //Setup using Timer library
  Timer1.initialize(1000000); //Initialize TimerONe to 1 second intervals
  Timer1.attachInterrupt(timerTick); //Attach timerTick function to the timer
  Timer1.stop(); //Stop timer until explicityly started
  Serial.begin(9600);
}


//MOTOR WILL KEEP OSCILLATING SO LONG AS THERE'S TIME REMAINING
//ONCE TIME RUNS OUT THE SEVEN SEG SHOULD DISPLAY 00:00 FOREVER
void loop(){
	// Improved button debounce and action handling
    bool currentIncrementState = digitalRead(incrementPin);
    bool currentDecrementState = digitalRead(decrementPin);
    bool currentStartState = digitalRead(startPin);
    bool currentStopState = digitalRead(stopPin);

    // Process increment button
    if (currentIncrementState != lastIncrementButtonState) {
        lastDebounceTime = millis();
    } else if ((millis() - lastDebounceTime) > debounceDelay && currentIncrementState == LOW) {
        incrementTime();
        lastDebounceTime = millis(); // Prevent immediate repeat
    }

    // Process decrement button
    if (currentDecrementState != lastDecrementButtonState) {
        lastDebounceTime = millis();
    } else if ((millis() - lastDebounceTime) > debounceDelay && currentDecrementState == LOW) {
        decrementTime();
        lastDebounceTime = millis();
    }

    // Process start button
    if (currentStartState != lastStartButtonState) {
        lastDebounceTime = millis();
    } else if ((millis() - lastDebounceTime) > debounceDelay && currentStartState == LOW) {
        startTime();
        lastDebounceTime = millis();
    }

    // Process stop button
    if (currentStopState != lastStopButtonState) {
        lastDebounceTime = millis();
    } else if ((millis() - lastDebounceTime) > debounceDelay && currentStopState == LOW) {
        stopTime();
        lastDebounceTime = millis();
    }

    // Update last states at the end of the loop
    lastIncrementButtonState = currentIncrementState;
    lastDecrementButtonState = currentDecrementState;
    lastStartButtonState = currentStartState;
    lastStopButtonState = currentStopState;

    // Your existing updateTime call remains here
    updateTime();
}



/*
Helper functions and ISRs
*/

void timerTick(){
	if(countDownActive){
		decrementTime();
		if(minutes == 0 && seconds == 0){
			stopTime(); //Stop the timer if it reaches 0
		}
	}
}


void startTime(){
	if(!countDownActive){
		countDownActive = true;
		digitalWrite(masterPin, HIGH); //Send output signal as HIGH
		Timer1.resume(); //Resume timer interrupt for decrementing the countdown
	}

}

void stopTime(){
	countDownActive = false;
	Timer1.stop(); //Stop the timer interrupt;
	digitalWrite(masterPin, LOW);
}


void timerInterrupt(){

}

//setup button pins
void buttonSetup(){
	for (int i=0; i < 4; i++){
	pinMode(buttonPins[i], INPUT_PULLUP); //set segment and DP pins to output
	}
	pinMode(masterPin, OUTPUT);
	digitalWrite(masterPin, LOW);
}


//USED FOR CLEARING SWING TIME
void clearTimer(){
  minutes = 0;
  seconds = 0;
}


//this function is responsible for time decrementing logic
//Function decrements timer by 1 SECOND
//THIS FUNCTION WILL BE CALLED BY ISR EVERY SECOND TO GET THE TIMING RIGHT
void decrementTime(){
	Serial.println("AH YOU PUSHED DECREMENT :0 +++++++++++++ ");
	noInterrupts(); //Disable interrupts

	if(seconds > 0){
		seconds--; //Countdown each second
	}else if(minutes > 0){
		minutes --; //Decrement minutes if seconds is already at 0
		seconds = 59; //Resets seconds to 59;
	}

	interrupts();
	updateTime();
}


void incrementTime(){
    Serial.println("AH YOU PUSHED INCREMENT :0 +++++++++++++ ");
    noInterrupts(); //Disable interrupts
	minutes++; //Increment minutes

	if(minutes >= 100){ //Check if minutes exceed 99
		minutes = 0;
	}
	interrupts();
}

void updateTime(){
	lcd.clear();
  	lcd.setCursor(0,0);
	lcd.print("********************");

	lcd.setCursor(0,1);
	lcd.print("**** SWING TIMER ****");

	lcd.setCursor(0,2);
	if(minutes > 10){
		lcd.print("******* ");
		lcd.print("0");
		lcd.print(minutes);
		lcd.print(":");
		lcd.print(seconds);
		lcd.print(" *******");
	}else{
		lcd.print("******* ");
		lcd.print(minutes);
		lcd.print(":");
		lcd.print(seconds);
		lcd.print(" *******");
	}

	lcd.setCursor(0,3);
	lcd.print("********************");
}

