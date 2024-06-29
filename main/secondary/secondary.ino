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


const int CLOCK_SPEED = 16000000;

// defines pins numbers
const short int masterPin = 2;
const short int stepPin = 3;
const short int dirPin = 4;
const short int enPin = 5;
const short int anglePins[] = {6, 7, 8, 9, 10, 11, 12, 13};

//MOTOR STRENGTH VARIABLE
//using the microsecondsDelay function
//valid min is 250
//valid max is 10000

//to make the motor move without being demeshed from the gears, set motor strength to at least 500
float motorStrength = 5000; // tried 7000, motor not turning but making noise..
// INCREASE VALUE BY 50 TO GET A QUATER CIRCLE OF MOTION AT MOTOR STRENGTH 965
const int SPIN_TIME = 90; // CONTROLS HOW LONG MOTOR WILL SPIN FOR
float swingPeriod = 1300;//desired period for swing motion 


const int MAX_ANGLE = -45;

const int ADXL345 = 0x53; // The ADXL345 sensor I2C address

//PRE-DEFINED ANGULAR VARIABLES
const int numAngles = 7;
const int angles[] = {0,15,30,45,60,75,80.95};
const int motorStrengths[] = {400, 500, 600, 700, 800};
const int DEFAULT_MOTOR_STRENGTH = 900;
const int WACKY_MOTOR_STRENGTH = -1;
const int minTurnOnAngle = 0;
const int maxTurnOnAngle = -15;

//ACCELEROMETER TEST VARIABLES  
const int ACCEL_TEST_NUM = 200;
const int MAX_ACCEL_FAILS = ACCEL_TEST_NUM/3;
const int startUpPositionCheckTime = 10;//(unit: seconds) how low to check Ava is in a valid startup posiiton.
int accelCounter = 0;//this variable counts to ACCEL_TEST_NUM in accelOffline
int accelFailCounter = 0;//this variable increments/resets in accelOffline
bool accelStartupCheckResult;




//ANGULAR MATH VARIABLES
float X_out, Y_out, Z_out;  // Outputs
float roll,pitch,rollF,pitchF=0;
float prevXAngle, xAngle, yAngle, zAngle;//holds angle in respective deminsion

int currMaxAngle = 0;//keeps track of max angle on return swings in quadrant 3
int prevRoll = 0;//used in movingFoward function to determine if the swing is moving foward

//Timer variables 
int secTick = 0;//used to count up to a second in interrupt
float secCounter = 0;//keep tracks of the number of seconds that's passed in motor timer interrupt

//used to determine if load should be pushed by angle based on timer based
//in the setup function
bool accelWorks = false;//keeps track if accelerometer works on startup


//setup LED pins to show what angle load is in
//USED FOR TESTING
void setupLED(){

  for(int i =0; i< sizeof(anglePins)/sizeof(short int); i++){
    pinMode(anglePins[i], OUTPUT);//set direction of LED pins
  }
}

//turnoff all leds
//USED FOR TESTING
void turnOffLED(){
  for(int i =0; i < sizeof(anglePins)/sizeof(short int); i++){
    digitalWrite(anglePins[i], LOW);
  }
}

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
  //prevXAngle = xAngle;
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
    yAngle = atan( Y_out / (sqrt(pow(X_out, 2) + pow(Z_out, 2))));
    zAngle = atan( sqrt(pow(X_out, 2) + pow(Y_out, 2)) / Z_out);

    xAngle *= 180.00;   yAngle *= 180.00;   zAngle *= 180.00;
    xAngle /= 3.141592; yAngle /= 3.141592; zAngle /= 3.141592;
}

//get roll and pitch from acceleration values
void getRollAndPitch() {

  prevRoll = rollF;//store the previous roll, used in movingFoward function.

  // Calculate Roll and Pitch (rotation around X-axis, rotation around Y-axis)
  pitch = atan(-1 * X_out / sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / PI;
  roll = atan(Y_out / sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / PI;
  // Low-pass filter
  rollF = 0.94 * rollF + 0.06 * roll;
  pitchF = 0.94 * pitchF + 0.06 * pitch;

}


//used to get the accel, angle, and roll/pitch in one go
//this is because getRollAndPitch rely on data from
//getAccel and getAngle to make calculations
//The same goes with getAngle and getAccel
void getOrientation(){
  getAccel();
  getAngle();
  getRollAndPitch();
}



//this function is used to determine if the
//the acceleteromter is in quadrant 3
//of the unit circle
//this maps to being between 0 and -90
bool inQuadrantThree(){
  if(rollF <= 0 && rollF > -90){//if in angle range
    return true; 
  }

  return false;
}

//this function is used to determine if the
//the acceleteromter is in quadrant 4
//of the unit circle
//this maps to being between 0 and 90
bool inQuadrantFour(){
  if(rollF > 0 && rollF <= 90){//if in angle range
    return true;
  }
  return false;//else return false
}

//determines if motor is in correct region to turn on.
//basically if it's in quadrant 4 of pi circle, its fine to move.
//specific angles will be fine turned
//return true if the load is in the right location
//return false if not
bool inMotorTurnOnZone (){
   //basically is saying if load is between 0 and -15, then
   //then its fine for the motors to kick on
   if(rollF <= minTurnOnAngle && rollF >= maxTurnOnAngle){
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
  if(prevRoll > rollF){
    //return true to indicate that load is moving forward
    return true;
  }

  //return false to indicate load is moving back
  return false;

}


//function for determining if acceleteromet is offline
//haven't worked out the logic for it yet
bool accelOffline(){
  accelCounter+=1;//increment Accelerometer Test counter
  
  if(accelCounter == ACCEL_TEST_NUM){
    accelCounter = 0;//reset the test counter
    accelFailCounter = 0;//reset the fail counter
  }


  if(xAngle = 0 && yAngle == 0 && zAngle == 0){
    accelFailCounter +=1;
  }
  if(accelFailCounter > accelCounter || accelFailCounter >= MAX_ACCEL_FAILS){
    return false;
  }

  return true;
}



//this function should only be used when the accelrometer is being used
//when the masterPin goes high. Wait a determined amount of time to make sure that
//Ava is in a safe startup position
bool accelStartupPositionCheck(){

  int validCounter = 0;//counter keep track of how many times Ava is in valid angle startup range
  int testNum = 1000;//counter max
  int passNum = testNum*0.95;//counter must reach this value to pass the test

  for(int i =0;i<testNum;i++){
    if(rollF > -15 && rollF < 15){//in valid range
          validCounter+=1;
    }

    //return true if Ava is deemed to be in a good position
    if(validCounter >= passNum){
      return true;
    }
  }

  return false;//by default
}


//use to check to see if the accelerometer is behaving corretly on startup
//if the accelerometer returns valid values X NUMBER OF times,
//its fine to drive the motors through accelerometer (so return true)
//otherwise return false so that interrupts can be setup up to drive
//motors based on timing intervals (period).
//returns true by default
bool checkAccelStartup(){

  //if the accelerometer isnt offline and Ava is rest
  //return true
  if(accelOffline() == true && accelStartupPositionCheck() ==true){
    return true;
  }

  return false;
}



//setup Motors
void setupMotors() {
    // Sets the two pins as Outputs
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, HIGH);


    pinMode(masterPin, INPUT);//pin used for recieved command form MASTER ARDUINO


}


//determine Motor stregnth based current angle
void determineMotorStrength(){
  
  //check to see what angle range motor is in and select motorStrength
  
  if(currMaxAngle < rollF){//updates the max angle if rollF is steeper
    currMaxAngle = rollF;
  }

  if(currMaxAngle < MAX_ANGLE){//angle is too steep then 
    motorStrength = WACKY_MOTOR_STRENGTH;//indicate the motor shouldnt shouldn't push by assigning meaningless value
  }

  else{
    for(int i = 0; i < numAngles-1; i++){
      if(rollF >= angles[i] && rollF < angles[i+1]){

        motorStrength = motorStrengths[i];//select motor strength

        //select led to indicate which angle range load is in
        //turnOffLED();//TURN OFF all LEDS
        digitalWrite(anglePins[i], HIGH);//turn on respective angle LED

        return;
      }
    }
  }
}

//Function that moves motors at specified strength
void moveMotors(){

   //cli();

   //do nothing it was determined in determine motor strength function
   //that Ava on the return swing max angle was greater than -45
   //if that was true, motor strength should have been set to 
   //WACKY_MOTOR_STRENGTH
   /*if(motorStrength == WACKY_MOTOR_STRENGTH){
      
      //setting the enable pin high on the microsteppr, prevents the motor from moving
      //setting the enable pin low on the microstepper, allows the motor to move
      //basically when the program counter is outside of this function
      //the enable pin should be High so that the motor doesn't accidently move
      digitalWrite(enPin, HIGH);//prevents the motor from moving
      return;//do nothing
   }*/

   //else{
      // Serial.println("Top of moveMotors()");
      digitalWrite(enPin, LOW);//allows the motor to move
      digitalWrite(dirPin, LOW); // Enables the motor to move in a particular direction


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
      digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction


      // THERE HAS TO BE AT MINIMUM A 2 SECOND DELAY, TO ENSURE THE MOTOR DOESNT MISFIRE IN THE WRONG DIRECTION
      // THERE IS SOME AMOUNT OF DELAY WHEN THE SIGNAL GOES THROUGH THE MICROSTEPPER
      // NEED TO GET GIVE SIGNALS MORE TIME TO PASS THROUGH BEFORE NEXT INSTRUCTION
      //delay(2000); // Two Second Delay
   //}

    digitalWrite(enPin, HIGH);//prevents the motor from moving

    // Serial.println("Bottom of moveMotors()");
}




//push ava based on what angle she currently is
void pushAvaAccel(){
      getAccel();//update acceleration values
      getAngle();//update angle values
      getRollAndPitch();//get roll and pitch values


      if(movingFoward() == false && inQuadrantThree() == true){
        determineMotorStrength();//determine motorStrength
      }
      //check to see if its okay to move motors
      if(inMotorTurnOnZone() == true && movingFoward() == true){
        moveMotors();//move the motors
      } 

}


//pushes Ava every few seconds
void pushAvaTime(){
    // Serial.println("Inside pushAvaTime.");
    moveMotors();
    delay(swingPeriod);
}

//returns status of master Pin
//which is a signal from the primary arduinio
//that tells secondary adruino it can still push 
//Ava because the timer hasn't decremented.
bool getMasterLine(){

  //return true if the pins high
  if(digitalRead(masterPin) == HIGH){
    return true;
  }

  //return false if the pins are low
  return false;
}




//sets everything up
//uses accelWorks() function to determine how motors
//should be triggered
void setup() {
  Serial.begin(9600); // Initiate serial communication for printing the results on the Serial monitor

  //setupLED();//setup LEDs
  setupAccel();//setup acceleterometer
  accelStartupCheckResult = checkAccelStartup();
  setupMotors();
/*  //this is the motor setup for interrupts
    //not using it right now
if(accelStartupCheckResult == false){
    setupMotors();//setup motors
    motorTimerSetup();
  }*/

  // else{
    //do nothing else for setting up
    //if Accel works
  // }

}


void loop() {

  moveMotors();
  delay(swingPeriod);
/*
  //functionality for how to move Ava when the acceleomter is working
  if(accelStartupCheckResult == true){
     while(true){
       //gets accel, angles, roll/pitch
       getOrientation();

       Serial.print("X angle: ");
       Serial.print(xAngle);
      
       Serial.print(" Y angle: ");
       Serial.print(yAngle);
      
       Serial.print(" Z angle: ");
       Serial.println(zAngle);

       Serial.print(rollF);
       Serial.print("/");
       Serial.println(pitchF);
  
       //pushAvaAccel();
     }
     
   }*/

  // //if it is determined accelerometer doesn't work
  // //push Ava using time based pushing
  // if(accelStartupCheckResult == false){
  //   while(getMasterLine()== true){
  //     pushAvaTime();
  //   }

  // }
  //pushAvaTime();

  //delay(1000);


  
}


//EVERYTHING BELOW MAIN IS NOT BEING USED
//WHICH IS BASICALLY THE INTERRUPT CODE
//SAVING IT FOR LATER IF ITS DETERMIEND TO BE USEFUL



//THIS IS USING TIMER0
//16 bit timer
void motorTimerSetup() {
  cli();
  // Clear registers
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 0;

  // 100.16025641025641 Hz (16000000/((155+1)*1024))
  //period is 0.01 seconds
  OCR0A = 255;

  // 744.047 Hz (16000000/((20+1)*1024))
  //Period is 0.001344 seconds
  //OCR0A = 30;
  // CTC
  TCCR0A |= (1 << WGM01);
  // Prescaler 1024
  TCCR0B |= (1 << CS02) | (1 << CS00);
  // Output Compare Match A Interrupt Enable
  TIMSK0 |= (1 << OCIE0A);
  sei();
}

//disble motor timer
void disableMotorInterrupt(){  
  //Disable Compare Match A Interrupt
  TIMSK0 |= (0 << OCIE0A);
}


//enable motor timer
void enableMotorInterrupt(){  
  //Disable Compare Match A Interrupt
  TIMSK0 |= (1 << OCIE0A);
}







//With the settings above, this IRS will trigger each 500ms.
ISR(TIMER1_COMPA_vect){
  //cli();//disable all interrupts

  secTick += 1;//increment tick counter
  Serial.println(secTick);
  //Resolution is 0.5 seonds
  if(secTick >= 50){//want the counter to go up to 50 so that at least a second has passed
    secCounter +=0.5;//increment second's counter
  }

  //need a function to re-enable timer0 functionality, because the delay functions use it
  //so i need to re-enable timer0 without triggering this interrupt

  //once swing period is achieved move the motor
  if(secCounter == swingPeriod){

    secCounter = 0;//reset second's counter
    //disableMotorInterrupt();
    //sei();
    moveMotors();
    //cli();
    //enableMotorInterrupt();
  }

  //sei();//re-enable all interrupts

}

