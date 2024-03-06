/*
* Project: Ava's Motorized Swing
* File:    accelRead.ino
* Authors: Marc, Jess, Yoel
*/


/*
    Arduino and ADXL345 Accelerometer - 3D Visualization Example 
     by Dejan, https://howtomechatronics.com
*/


//THIS IS USING ARDUINO UNO
#include <Wire.h>  // Wire library - used for I2C communication
#include <time.h>


// defines pins numbers
const short int masterPin = 2;
const short int stepPin = 3;
const short int dirPin = 4;
const short int enPin = 5;
const short int anglePins[] = {6, 7, 8, 9, 10, 11, 12, 13, 14};

// INCREASE VALUE BY 50 TO GET A QUATER CIRCLE OF MOTION AT MOTOR STRENGTH 965
const int SPIN_TIME = 400; // CONTROLS HOW LONG MOTOR WILL SPIN FOR


const int ADXL345 = 0x53; // The ADXL345 sensor I2C address

const int numAngles = 6;
const int angles[] = {-90, -75,-60, -45,-30,-15, 0};
const int motorStrengths[] = {400, 500, 600, 700, 800};

float motorStrength = 0;
float X_out, Y_out, Z_out;  // Outputs
float roll,pitch,rollF,pitchF=0;
float prevXAngle, xAngle, yAngle, zAngle;//holds angle in respective deminsion


void setupAccel() {
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // Bit D3 High for measuring enable (8dec -> 0000 1000 binary)
  Wire.endTransmission();
  delay(10);

  offsetCalibration();

}

void offsetCalibration (){
 //Off-set Calibration
  //X-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1E);
  Wire.write(1);
  Wire.endTransmission();
  delay(10);
  //Y-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1F);
  Wire.write(-2);
  Wire.endTransmission();
  delay(10);

  //Z-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x20);
  Wire.write(-9);
  Wire.endTransmission();
  delay(10);
}

void getAccel(){
  //store previous angles, before geting new acceleration
  prevXAngle = xAngle;
  //prevYAngle = Y_out;//dont need this
  //prevZAngle = Z_out;//dont need this   

   // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read() | Wire.read() << 8); // X-axis value
  X_out = X_out / 256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read() | Wire.read() << 8); // Y-axis value
  Y_out = Y_out / 256;
  Z_out = ( Wire.read() | Wire.read() << 8); // Z-axis value
  Z_out = Z_out / 256;

}

//This function returns angle in degrees from the X, Y, and Z directions
//We want to pay attention to X for the swing motion.
void getAngle (){
  
    xAngle = atan( X_out / (sqrt(pow(Y_out, 2) + pow(Z_out, 2))));
    //yAngle = atan( Y_out / (sqrt(pow(X_out, 2) + pow(Z_out, 2))));
    //zAngle = atan( sqrt(pow(X_out, 2) + pow(Y_out, 2)) / Z_out);

    xAngle *= 180.00;   //yAngle *= 180.00;   zAngle *= 180.00;
    xAngle /= 3.141592; //yAngle /= 3.141592; zAngle /= 3.141592;
}

//get roll and pitch from acceleration values
void getRollAndPitch() {
  // Calculate Roll and Pitch (rotation around X-axis, rotation around Y-axis)
  pitch = atan(-1 * X_out / sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / PI;
  roll = atan(Y_out / sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / PI;
  // Low-pass filter
  rollF = 0.94 * rollF + 0.06 * roll;
  pitchF = 0.94 * pitchF + 0.06 * pitch;

}

//determines if motor is in correct region to turn on.
//basically if it's in quadrant 4 of pi circle, its fine to move.
//specific angles will be fine turned
//return true if the load is in the right location
//return false if not
bool inMotorTurnOnZone (){
   if(xAngle <= 0 && xAngle >= -90){
    return true;
   }

    return false;
}

//determines if load is moving forwards or backwards
//if moving backwards return 0
//if moving forwards return 1
bool movingFoward(){

  //xAngle should be getting closer to 0 if it's moving foward
  //xAngle should always have a negative value
  //so the closer xAngle is to 0, the more Ava will face up towards the sky 
  if(prevXAngle < xAngle){
    //return true to indicate that load is moving forward
    return true;
  }

  //return false to indicate load is moving back
  return false;

}

//setup Motors
void setupMotors() {
    // Sets the two pins as Outputs
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, LOW);


    pinMode(masterPin, INPUT);//pin used for recieved command form MASTER ARDUINO


}


//determine Motor stregnth based current angle
void determineMotorStrength(){
  
  //check to see what angle range motor is in and select motorStrength
  for(int i = 0; i < numAngles-1; i++){
    if(xAngle >= angles[i] && xAngle < angles[i+1]){

       motorStrength = motorStrengths[i];//select motor strength

       //select led to indicate which angle range load is in
       turnOffLED();//TURN OFF all LEDS
       digitalWrite(anglePins[i], HIGH);//turn on respective angle LED

      return;
    }
  }

  //some error checking code should go here
  //if angle isn't within proper swing motion range in quadrants 3 and 4
  //something is wrong and motors should be turned off and program should be stopped.

}


//Function that moves motors at specified strength
void moveMotors(){

   //cli();
    digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction
    for (int x = 0; x < SPIN_TIME; x++)
    { // SPIN_TIME VALUE DICTATES HOW LONG MOTOR WILL SPIN
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(motorStrength);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(motorStrength);
    }

    // THIS DELAY IS WEIRD, BUT NEED TO BE INCLUDED OR ELSE THE CODE WONT WORK
    // REASON UNCLEAR
    delay(10);                 // THIS NEEDS TO BE HERE

    
    //PUT ANOTHER MOTOR MOVE LOOP HERE IF you want
    //But for now disable motors until next function call
    digitalWrite(dirPin, LOW); // Enables the motor to move in a particular direction


    // THERE HAS TO BE AT MINIMUM A 2 SECOND DELAY, TO ENSURE THE MOTOR DOESNT MISFIRE IN THE WRONG DIRECTION
    // THERE IS SOME AMOUNT OF DELAY WHEN THE SIGNAL GOES THROUGH THE MICROSTEPPER
    // NEED TO GET GIVE SIGNALS MORE TIME TO PASS THROUGH BEFORE NEXT INSTRUCTION
    delay(2000); // Two Second Delay

}

//setup LED pins to show what angle load is in
void setupLED(){
  for(int i =0; i< sizeof(anglePins)/sizeof(short int); i++){
    pinMode(anglePins[i], OUTPUT);
  }
}

//turnoff all leds
void turnOffLED(){
  for(int i =0; i < sizeof(anglePins)/sizeof(short int); i++){
    digitalWrite(anglePins[i], LOW);
  }
}



//sets everything up
void setup() {
  Serial.begin(9600); // Initiate serial communication for printing the results on the Serial monitor
  setupLED();//setup LEDs
  setupAccel();//setup acceleterometer
  setupMotors();//setup motors
  
}

void loop() {
  
  Serial.print("X angle: ");
  Serial.println(xAngle);
  /*
  Serial.print(" Y angle: ");
  Serial.print(yAngle);
  Serial.print(" Z angle: ");
  Serial.println(zAngle);
  */


  getAccel();//update acceleration values
  getAngle();//update angle values

  //check to see if its okay to move motors
  if(inMotorTurnOnZone() == true && movingFoward() == true){
    determineMotorStrength();//determine motorStrength
    moveMotors();//move the motors
  }
  delay(1000);

  /*
  Serial.print(roll);
  Serial.print("/");
  Serial.println(pitch);
  */
}
