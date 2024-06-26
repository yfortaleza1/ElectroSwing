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
const short int masterPin = 12;
const short int buttonPins[] = {incrementPin, decrementPin, startPin, stopPin};//used for buttons

//Define global timer variables
volatile int minutes = 0;
volatile int seconds = 0;
volatile bool countDownActive = false;
volatile bool isAtStartup = true;
volatile bool timeIsSet = false;

// Global variables for debounce
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long clearDelay = 500; //the extra time for a clear to be registered

// Variables to store the button states (this is for reading debounces)
bool lastIncrementButtonState = HIGH;
bool lastDecrementButtonState = HIGH;
bool lastStartButtonState = HIGH;
bool lastStopButtonState = HIGH;
bool lastClearButtonState = HIGH;

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
	//setup
	buttonSetup();
	clearTimer();

	//LCD initialization
	lcd.init();
	lcd.backlight();
	lcd.clear();

	//Continued setup
	printStaticMessage();
	updateTime();

	//Attaching interrupt pins to ISRs
	attachInterrupt(digitalPinToInterrupt(stopPin), stopTime, CHANGE);
	attachInterrupt(digitalPinToInterrupt(startPin), startTime, CHANGE);

	//Setup for using Timer library
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
        if (currentMillis - lastDebounceTime > debounceDelay + clearDelay && currentStopState == LOW) {
            clearTimer();
        }
        lastDebounceTime = currentMillis; // Update the debounce timer
    }

    // Update the last known states
    lastIncrementButtonState = currentIncrementState;
    lastDecrementButtonState = currentDecrementState;
    lastStartButtonState = currentStartState;
    lastStopButtonState = currentStopState;

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
	
	if (!countDownActive && timeIsSet && (minutes > 0 || seconds > 0)) { // Additional check for actual time
        countDownActive = true;
		Timer1.resume(); // Start countdown
        digitalWrite(masterPin, HIGH); // Signal that countdown is active
    }
	

}

void stopTime(){
	if(timeIsSet){
		countDownActive = false;
		Timer1.stop(); //Stop the timer interrupt;
    Serial.println("Timer Stopped");
		digitalWrite(masterPin, LOW);
	}
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
	noInterrupts();
	Timer1.stop();
	minutes = 0;
	seconds = 0;
	timeIsSet = false;
	isAtStartup = true;
	interrupts();
	updateTime();
}


//this function is responsible for time decrementing logic
//Function decrements timer by 1 SECOND
//THIS FUNCTION WILL BE CALLED BY ISR EVERY SECOND TO GET THE TIMING RIGHT
void decrementTime(){
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
	noInterrupts(); // Disable interrupts
    // Calculate the total time in seconds
    int totalTimeInSeconds = minutes * 60 + seconds;

    // Check to prevent underflow and ensure total time is at least 30 seconds before decrementing
    if (totalTimeInSeconds >= 30) {
        totalTimeInSeconds -= 30; // Decrement by 30 seconds

        // Convert back to minutes and seconds
        minutes = totalTimeInSeconds / 60;
        seconds = totalTimeInSeconds % 60;
        
        timeIsSet = true; // Indicate that time has been set or adjusted
        timeIsSet = true; // Indicate that time has been set or adjusted
    }
    interrupts();
    updateTime();
}

void incrementTime() {
    noInterrupts(); // Disable interrupts
    if (isAtStartup) {
        minutes = 0; // Initialize minutes if it's at startup
        isAtStartup = false;
    } else {
        if (minutes < 20) { // Only increment if below 20 minutes
            minutes++;
        }
    }
    interrupts();
    timeIsSet = true; // Mark that time has been set
    updateTime(); // Update the display
}

void updateTime(){
    lcd.setCursor(8, 2);
    if (minutes < 10) {
        lcd.print("0"); // Leading zero for minutes
    }
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) {
        lcd.print("0"); // Leading zero for seconds
    }
    lcd.print(seconds);
    lcd.print(" "); // Clear any leftover characters from previous displays
}


void printStaticMessage(){
	lcd.setCursor(0,0);
	lcd.print("********************");

	lcd.setCursor(0,1);
	lcd.print("**** AVA'S SWING ****");

	lcd.setCursor(0,2);
	lcd.print("****");

	lcd.setCursor(15,2);
	lcd.print(" ****");

	lcd.setCursor(0,3);
	lcd.print("********************");
}
