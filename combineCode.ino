//THIS IS USING ATMEGA2560 
//ONLY WORKS FOR THE MEGA REV3, THE ONE WE ARE PLANNING ON USING FOR THE FINAL PRODUCT
//THIS CODE WON'T WORK ON THE UNO BECCAUSE OF DIFFERENT REGISTERS AND CLOCK SPEED (NEED TO CONFIRM)

#include <time.h>
#include <Stepper.h>
#include "C:\Users\Marc\Desktop\LIS3DHTR.h"
#include "C:\Users\Marc\Desktop\LIS3DHTR.cpp"

#include <Wire.h>
#include "C:\Users\Marc\Desktop\DigitalAccelerometer_ADXL345\DigitalAccelerometer_ADXL345\ADXL345.cpp"
#include "C:\Users\Marc\Desktop\DigitalAccelerometer_ADXL345\DigitalAccelerometer_ADXL345\ADXL345.h"



const int sensorPin= 0;//The analog sensor is connected to analog pin 0 of the arduino

//ABCDEFG,dp
const int numeral[10]= {
B00010100, //0
B11010111, //1
B01001100, //2
B01000101, //3
B10000111, //4
B00100101, //5
B00100100, //6
B01010111, //7
B00000100, //8
B00000101, //9
};

//ASSUMING THE CLOCK IS 8MHZ

short int secTicks = 0;//used to keep track when a second should occur, clock is 16MHZ, so count up to 16 mill and decremenet timer
short int minTens = 0;//tens place for minutes
short int minOnes = 0;//ones place for minutes
short int secTens = 0;//tens place for seconds
short int secOnes = 0;//ones place for seconds

//BUTTON PINs
const short int incrementPin = 22;//used for incrementing time
const short int decrementPin = 23;//used for decremnting time
const short int startPin = 24;//used for starting swing time
const short int stopPin = 25;//used for clearing and stoping swing time


bool motorIdle = false;



//pins for decimal point and each segment
//dp, G, F, E, D, C, B, A
const short int segmentPins[]= { 2, 3, 4, 5, 6, 8, 11, 12};///used for 7 segment
const short int buttonPins[] = {incrementPin,decrementPin,startPin,stopPin};;//used for buttons

const short int numberofDigits=4;

const short int digitPins[numberofDigits] = { 7,9,10,13}; //digits 1, 2, 3, 4



//ACCEL VARIABLES
const int sdaPin = A4; //analog input 4 -- ground
const int sdlPin = A5; //analog input 5 -- voltage
const int xPin = A3; //x-axis
const int yPin = A2; //y-axis
const int zPin = A1; //z-axis


//setup button pins
void buttonSetup(){
  for (int i=0; i < 4; i++){
    pinMode(buttonPins[i], INPUT_PULLUP); //set segment and DP pins to output
  }

}



//FUCTION SETUPS CLOCK INTERRUPTS
//8 BIT TIMER
//USES TIMER1
void clockInterruptSetup(){

  cli();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 1 Hz (16000000/((15624+1)*1024))
  //HAVING OCR1A EQUAL 15624 MAKES THE INTERRUPT TRIGGER ONCE EVERY SECOND
  //IF YOU WANT TO SPEED DECREMENTING MAKE OCR1A SMALLER THAN 15624
  OCR1A = 15624;
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // Output Compare Match A Interrupt Enable
  // Marc - This Mask is used to enable the interrupts for a specific thing. To disable it, just change the 1 in the below
  // equation to a 0. That way you can turn off specific interrupts instead of just disabling everything.
  TIMSK1 |= (1 << OCIE1A);
  sei();

}


volatile int divider=0;
volatile int dividerMotor=0;


//USED THIS FOR TRIGGERING SEGMENT DISPLAY
//THIS IS USING TIMER0
//16 bit timer
void SevenSegTimerSetup() {
  cli();
  // Clear registers
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 0;

  // 100.16025641025641 Hz (16000000/((155+1)*1024))
  //period is 0.01 seconds
  //OCR0A = 155;

  // 744.047 Hz (16000000/((20+1)*1024))
  //Period is 0.001344 seconds
  OCR0A = 20;
  // CTC
  TCCR0A |= (1 << WGM01);
  // Prescaler 1024
  TCCR0B |= (1 << CS02) | (1 << CS00);
  // Output Compare Match A Interrupt Enable
  TIMSK0 |= (1 << OCIE0A);
  sei();
}


//USED THIS FOR TRIGGERING SEGMENT DISPLAY
//THIS IS USING TIMER2
//8 bit timer
void MotorClockSetup() {
  cli();
  // Clear registers
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  // 100.16025641025641 Hz (16000000/((155+1)*1024))
  //period is 0.0092 seconds
  //HAVE A COUNTER THAT IS TRIGGERED BY THIS INTERRUPT
  OCR2A = 154;
  // CTC
  TCCR2A |= (1 << WGM01);
  // Prescaler 1024
  TCCR2B |= (1 << CS02) | (1 << CS00);
  // Output Compare Match A Interrupt Enable
  TIMSK2 |= (1 << OCIE0A);
  sei();
}


//FUNCTION STEPS UP I/O PINS
void segmentPinSetup(){
  for (int i=0; i < 8; i++){
    pinMode(segmentPins[i], OUTPUT); //set segment and DP pins to output
  }
  //sets the digit pins as outputs
  for (int i=0; i < numberofDigits; i++){
    pinMode(digitPins[i], OUTPUT);
  }


}


//This function is used to make sure that a pin isn't read multiple times
//After a pin is read that the opereator presses
//call this function
//it'll basically until the operator lets go off said button
void debounceButton(short int pinNumber){

  while(digitalRead(pinNumber) != LOW){
    //DO NOTHING BUT DISPLAY TIME
    displayTime();//display current selected time
    //stay in this while loop until the pin in question goes high again
  }

}


  
//USED FOR CLEARING SWING TIME
void clearTimer(){
  secOnes = 0;
  secTens = 0;
  minOnes = 0;
  minTens = 0;
}

void displayTime(){

  showDigit (minTens, 4);//show 4th digit
  showDigit (minOnes, 3);//show 3rd digit
  showDigit (secTens, 2);//show 2nd digit
  showDigit (secOnes, 1);//show 1st digit
}

//this function is responsible for time decrementing logic
//Function decrements timer by 1 SECOND
//THIS FUNCTION WILL BE CALLED BY ISR EVERY SECOND TO GET THE TIMING RIGHT
void decrementTimer(){

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
}






//Displays given number on a 7-segment display at the given digit position
void showDigit (int number, int digit){

  if(digit == 1){//MAKE ONLY THE FIRST DIGIT VISIBLE
    digitalWrite(digitPins[0], LOW);
    digitalWrite(digitPins[1], HIGH);
    digitalWrite(digitPins[2], HIGH);
    digitalWrite(digitPins[3], HIGH);
  }

  else if(digit == 2){//MAKE ONLY THE SECOND DIGIT VISIBLE
    digitalWrite(digitPins[0], HIGH);
    digitalWrite(digitPins[1], LOW);
    digitalWrite(digitPins[2], HIGH);
    digitalWrite(digitPins[3], HIGH);
  }

  else if(digit == 3){//MAKE ONLY THE THIRD DIGIT VISIBLE
    digitalWrite(digitPins[0], HIGH);
    digitalWrite(digitPins[1], HIGH);
    digitalWrite(digitPins[2], LOW);
    digitalWrite(digitPins[3], HIGH);
  }

  else if(digit == 4){//MAKE ONLY THE FOURTH DIGIT VISIBLE
    digitalWrite(digitPins[0], HIGH);
    digitalWrite(digitPins[1], HIGH);
    digitalWrite(digitPins[2], HIGH);
    digitalWrite(digitPins[3], LOW);
  }

  
  //digitalWrite(digitPins[digit], HIGH);//selects digit to have number displayed on
  
  for (int segment= 0; segment < 8; segment++){
    boolean isBitSet= bitRead(numeral[number], segment);
    isBitSet= ! isBitSet; //remove this line if common cathode display
    digitalWrite(segmentPins[segment], isBitSet);
  }

  //the delay below can be any value greater than 0
  //keep in mind the digits wont appear lit up at the same time
  //if its made too long
  delay(5);//change the delay to affect have fast leds scroll

  digitalWrite(digitPins[digit], LOW);

  /*//THIS CODE WILL MAKE 7SEG FLICKER WHEN TIMER RUNS OUT
  if(secOnes == 0 && secTens == 0 && minOnes == 0 && minTens == 0){
    delay(10);//THIS WILL CAUSE LEDS TO VISIBLY FLICKER, MAY BE ANNOYING :(
  }*/
  
}

// defines pins numbers
const short int stepPin = 26;
const short int dirPin = 27;
const short int enPin = 28;

// INCREASE VALUE BY 50 TO GET A QUATER CIRCLE OF MOTION AT MOTOR STRENGTH 965
const int SPIN_TIME = 50; // CONTROLS HOW LONG MOTOR WILL SPIN FOR

// FOR MOTOR STRENGTH 350 MINIMUM
// 940 IS CONSISTENT WITH STARTING AND STOPPING IN THE SAME PLACE, WHEN ROTATING LEFT AND BACK TO RIGHT
// DONT GO ABOVE 965 FOR MOTOR STRENGTH
const int MOTOR_STRENGTH = 800; // THIS IN A DELAY VALUE IN MICROSECONDS, LONGER THE DEPLAY SLOWER THE MOTOR WILL BE

void motorRotatorSetup() {
    // Sets the two pins as Outputs
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, LOW);

    pinMode(stepPin, OUTPUT);


}



void motorRotatorLoop() {

    //cli();
    digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction
    for (int x = 0; x < SPIN_TIME; x++)
    { // SPIN_TIME VALUE DICTATES HOW LONG MOTOR WILL SPIN
        //cli();
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(MOTOR_STRENGTH);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(MOTOR_STRENGTH);
        //displayTime();
        //sei();
    }

    // THIS DELAY IS WEIRD, BUT NEED TO BE INCLUDED OR ELSE THE CODE WONT WORK
    // REASON UNCLEAR
    delay(10);                 // THIS NEEDS TO BE HERE
    digitalWrite(dirPin, LOW); // Changes the rotations direction

    // THERE HAS TO BE AT MINIMUM A 2 SECOND DELAY, TO ENSURE THE MOTOR DOESNT MISFIRE IN THE WRONG DIRECTION
    // THERE IS SOME AMOUNT OF DELAY WHEN THE SIGNAL GOES THROUGH THE MICROSTEPPER
    // NEED TO GET GIVE SIGNALS MORE TIME TO PASS THROUGH BEFORE NEXT INSTRUCTION
    delay(2000); // Two Second Delay
    

    /*for (int x = 0; x < SPIN_TIME; x++)
    { // SPIN_TIME VALUE DICTATES HOW LONG MOTOR WILL SPIN
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(MOTOR_STRENGTH);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(MOTOR_STRENGTH);
    }
    // THIS DELAY IS WEIRD, BUT NEED TO BE INCLUDED OR ELSE THE CODE WONT WORK
    // REASON UNCLEAR
    delay(10);

    // BELOW IS CODE FOR MOVEMENT IN THE OPPOSITE DIRECTION
    digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction

    // THERE HAS TO BE AT MINIMUM A 2 SECOND DELAY, TO ENSURE THE MOTOR DOESNT MISFIRE IN THE WRONG DIRECTION
    // THERE IS SOME AMOUNT OF DELAY WHEN THE SIGNAL GOES THROUGH THE MICROSTEPPER
    // NEED TO GET GIVE SIGNALS MORE TIME TO PASS THROUGH BEFORE NEXT INSTRUCTION
    delay(2000);*/

    //sei();
}






void getSwingTime(){

  while(digitalRead(startPin) != HIGH){//be in this "SET TIME" MODE while start pin is low


    //IF THE THE STOP BUTTON IS PRESSED
    if(digitalRead(stopPin) != LOW){//clear time
      clearTimer();//clear time
      debounceButton(stopPin);//wait until stopPin is let go
    }

    //IF THE INCREMENT BUTTON IS PRESSED
    if(digitalRead(incrementPin) != LOW){

      debounceButton(incrementPin);//wait until incrementPin is let go

      if(minOnes < 9 && minOnes >= 0){
        minOnes +=1;//increment minsOnes place
      }

      else if(minOnes == 9 && minTens != 5){//ONES PLACE OVERFLOW CONDITION
        minOnes = 0;//set ones place of mines to 0
        minTens += 1;//incrmeent tens place of minutes
      }

    }


    //IF THE DECREMENT BUTTON IS PRESSED
    if(digitalRead(decrementPin) != LOW){

      debounceButton(decrementPin);//wait until incrementPin is let go

      if(minOnes > 0 && minOnes <= 9){
        minOnes -=1;//decrement minsOnes place
      }

      else if(minOnes == 0 && minTens != 0){//DECREMENTING TENS PLACE BY 1
        minOnes = 9;//set ones place of mines to 0
        minTens -= 1;//incrmeent tens place of minutes
      }

    }

        displayTime();//display current selected time
  }

  debounceButton(startPin);//wait until start pin is let go

}



//ACCELEROMETER STUFF

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library


//SETS UP PINS FOR ACCELEMOETER
void accelSetup(){


  Serial.begin(9600);
  adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);

}


/*
* Displays accelerometer reading at every given time interval 
*/
void getAccel(){
  
	//Boring accelerometer stuff   
	int x,y,z;  
	adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
	// Output x,y,z values 
	Serial.print("values of X , Y , Z: ");
	Serial.print(x);
	Serial.print(" , ");
	Serial.print(y);
	Serial.print(" , ");
	Serial.println(z);
	
	double xyz[3];
	double ax,ay,az;
	adxl.getAcceleration(xyz);
	ax = xyz[0];
	ay = xyz[1];
	az = xyz[2];
	Serial.print("X=");
	Serial.print(ax);
    Serial.print(" g");
	Serial.print("Y=");
	Serial.print(ay);
    Serial.print(" g");
	Serial.print("Z=");
	Serial.print(az);
    Serial.println(" g");
	Serial.println("**********************");
	//delay(500);
/*

  //Fun Stuff!    
  //read interrupts source and look for triggerd actions
  
  //getInterruptSource clears all triggered actions after returning value
  //so do not call again until you need to recheck for triggered actions
   byte interrupts = adxl.getInterruptSource();
  
  // freefall
  if(adxl.triggered(interrupts, ADXL345_FREE_FALL)){
    Serial.println("freefall");
    //add code here to do when freefall is sensed
  } 
  
  //inactivity
  if(adxl.triggered(interrupts, ADXL345_INACTIVITY)){
    Serial.println("inactivity");
     //add code here to do when inactivity is sensed
  }
  
  //activity
  if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
    Serial.println("activity"); 
     //add code here to do when activity is sensed
  }
  
  //double tap
  if(adxl.triggered(interrupts, ADXL345_DOUBLE_TAP)){
    Serial.println("double tap");
     //add code here to do when a 2X tap is sensed
  }
  
  //tap
  if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)){
    Serial.println("tap");
     //add code here to do when a tap is sensed
  } */
}



void setup()
{

 
  //USED FOR TESTING
  
  minTens = 1;//tens place for minutes
  minOnes = 3;//ones place for minutes
  secTens = 1;//tens place for seconds
  secOnes = 2;//ones place for seconds
  

  
  motorIdle = true;
  buttonSetup();//.setup button pins
  segmentPinSetup();//setup 7 segment pins
  getSwingTime();//get swing time from opereator via physical timer interface
  clockInterruptSetup();//setup clock interrupts


  SevenSegTimerSetup();
  motorRotatorSetup();//setup motors
  accelSetup();
  //motorIdle = false;

}





//MOTOR WILL KEEP OSCILLATING SO LONG AS THERE'S TIME REMAINING
//ONCE TIME RUNS OUT THE SEVEN SEG SHOULD DISPLAY 00:00 FOREVER
void loop(){
  //displayTime();
  getAccel();
  //motorRotatorLoop();//move motors

  /*if(secOnes != 0 || secTens != 0 && minOnes != 0 || minTens != 0){
      motorIdle = false;      
  }

  else{
    motorIdle == true;
  }*/
}





//With the settings above, this IRS will trigger each second.
//USED FOR DECREMENTING THE TIMER EVERY SECOND
ISR(TIMER1_COMPA_vect){
    cli();
    decrementTimer();//DECREMENT TIMER BY A SECOND
    sei();
    //displayTime();

}


//tTHIS HANDLES THE TRIGGERING THE SEVEN SEGMENT DISPLAY
//USED TIMER 0
ISR(TIMER0_COMPA_vect) {
  cli();//disable interrupts
  if(divider==0){
    displayTime();
    //getAccel();
  }
  divider++;
  divider%=100;
  sei();//reenable all interrupts
}



//tTHIS HANDLES THE MOTOR being rotating
//USED TIMER 2
ISR(TIMER2_COMPA_vect) {

  
  cli();//disable interrupts
  if(motorIdle == false){//MOVE MOTOR
    motorRotatorLoop();
  }

  else{
    digitalWrite(stepPin, HIGH);
  }
  dividerMotor++;
  dividerMotor%=80;//
  sei();//reenable all interrupts
}
