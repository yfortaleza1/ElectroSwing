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
void printStaticMessage();

void setup(){
	//USED FOR TESTING
	buttonSetup();
	clearTimer();

	lcd.init();
	lcd.backlight();
	lcd.clear();

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
void loop() {
    // Read the current state of buttons
    bool currentIncrementState = digitalRead(incrementPin);
    bool currentDecrementState = digitalRead(decrementPin);
    bool currentStartState = digitalRead(startPin);
    bool currentStopState = digitalRead(stopPin);

    unsigned long currentMillis = millis();

    // Check each button for a state change from HIGH to LOW (button press)
    if (currentIncrementState != lastIncrementButtonState) {
        if (currentMillis - lastDebounceTime > debounceDelay && currentIncrementState == LOW) {
            incrementTime();
        }
        lastDebounceTime = currentMillis; // Update the debounce timer
    }

    if (currentDecrementState != lastDecrementButtonState) {
        if (currentMillis - lastDebounceTime > debounceDelay && currentDecrementState == LOW) {
            decrementPresetTime();
        }
        lastDebounceTime = currentMillis; // Update the debounce timer
    }

    if (currentStartState != lastStartButtonState) {
        if (currentMillis - lastDebounceTime > debounceDelay && currentStartState == LOW) {
            startTime();
        }
        lastDebounceTime = currentMillis; // Update the debounce timer
    }

    if (currentStopState != lastStopButtonState) {
        if (currentMillis - lastDebounceTime > debounceDelay && currentStopState == LOW) {
            stopTime();
        }
        lastDebounceTime = currentMillis; // Update the debounce timer
    }

    // Update the last known states
    lastIncrementButtonState = currentIncrementState;
    lastDecrementButtonState = currentDecrementState;
    lastStartButtonState = currentStartState;
    lastStopButtonState = currentStopState;

    updateTime(); // Refresh the display with the current timer value
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

void decrementPresetTime(){ //This function will only be called when the countdown is stalled/stopped
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
	// Assuming your display format always occupies the same amount of space,
    // you can clear just the specific part of the display being updated to minimize flicker.
    lcd.setCursor(0, 2); // Position cursor at the start of the row for time display
    lcd.print("                "); // Clear the row with spaces

    // Now print the time in the same position
    lcd.setCursor(0, 2);
	lcd.print("******");
    if (minutes < 10) {
        lcd.print("0"); // Leading zero for minutes
    }
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) {
        lcd.print("0"); // Leading zero for seconds
    }
    lcd.print(seconds);
	lcd.print("*****");
    lcd.print(" "); // Clear any leftover characters from previous displays
}

void printStaticMessage(){
	
}

