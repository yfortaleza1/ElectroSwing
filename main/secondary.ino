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






//ACCEL VARIABLES
const int sdaPin = A4; //analog input 4 -- ground
const int sdlPin = A5; //analog input 5 -- voltage
const int xPin = A3; //x-axis
const int yPin = A2; //y-axis
const int zPin = A1; //z-axis

const short int masterPin = 2;


volatile int dividerMotor=0;



//USED THIS FOR TRIGGERING SEGMENT DISPLAY
//THIS IS USING TIMER2
//8 bit timer
void MotorClockSetup() {
  cli();
  TCCR1A = 0;           // Init Timer1
  TCCR1B = 0;           // Init Timer1
  TCCR1B |= B00000011;  // Prescalar = 64
  TCNT1 = 40535;        // Timer Preloading
  TIMSK1 |= B00000001;  // Enable Timer Overflow Interrupt
  sei();
}




 

// defines pins numbers
const short int stepPin = 3;
const short int dirPin = 4;
const short int enPin = 5;

// INCREASE VALUE BY 50 TO GET A QUATER CIRCLE OF MOTION AT MOTOR STRENGTH 965
const int SPIN_TIME = 400; // CONTROLS HOW LONG MOTOR WILL SPIN FOR

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


    pinMode(masterPin, INPUT);//pin used for recieved command form MASTER ARDUINO


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
    getAccel();

    // THERE HAS TO BE AT MINIMUM A 2 SECOND DELAY, TO ENSURE THE MOTOR DOESNT MISFIRE IN THE WRONG DIRECTION
    // THERE IS SOME AMOUNT OF DELAY WHEN THE SIGNAL GOES THROUGH THE MICROSTEPPER
    // NEED TO GET GIVE SIGNALS MORE TIME TO PASS THROUGH BEFORE NEXT INSTRUCTION
    delay(2000); // Two Second Delay
    

    for (int x = 0; x < SPIN_TIME; x++)
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
    delay(2000);
    getAccel();


}









//ACCELEROMETER STUFF

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library


//SETS UP PINS FOR ACCELEMOETER
void accelSetup(){
  //cli();

  /*TCCR1A = 0;           // Init Timer1A
  TCCR1B = 0;           // Init Timer1B
  TCCR1B |= B00000011;  // Prescaler = 64
  TCNT1 = 40535;        // Timer Preloading
  TIMSK1 |= B00000001;  // Enable Timer Overflow Interruptsei();*/

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
  //sei();
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


void setup(){

  motorRotatorSetup();//setup motors
  
  accelSetup();
  //motorIdle = false;
  pinMode(masterPin, INPUT);

}

//MOTOR WILL KEEP OSCILLATING SO LONG AS THERE'S TIME REMAINING
//ONCE TIME RUNS OUT THE SEVEN SEG SHOULD DISPLAY 00:00 FOREVER
void loop(){
  if(digitalRead(masterPin) == HIGH){
    motorRotatorLoop();
  }
  //motorRotatorLoop();
  //getAccel();

  
}


ISR(TIMER1_OVF_vect)
{
  TCNT1 = 40535; // Timer Preloading
  // Handle The 100ms Timer Interrupt
  //...
  //cli();
  getAccel();
  //sei();
}
