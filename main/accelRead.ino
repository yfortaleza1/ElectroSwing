/*
* Project: Ava's Motorized Swing
* File:    accelRead.ino
* Authors: Yoel Fortaleza
* Date:    Mon. 11/20/2023
*/

const int sdaPin = A4; //analog input 4 -- ground
const int sdlPin = A5; //analog input 5 -- voltage
const int xPin = A3; //x-axis
const int yPin = A2; //y-axis
const int zPin = A1; //z-axis

void setup() {
  // put your setup code here, to run once:

  // initialize serial communication
  Serial.begin(9600);

  //Provide ground and power using analog inputs as normal digital pins
  pinMode(sdaPin, OUTPUT);
  pinMode(sdlPin, OUTPUT);

  digitalWrite(sdaPin, LOW);
  digitalWrite(sdlPin, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

  //print the sensor values:
  Serial.print(analogRead(xPin));
  Serial.print("\r");
  Serial.print("\n");
  

  Serial.print(analogRead(yPin));
  Serial.print("\r");
  Serial.print("\n");

  Serial.print(analogRead(zPin));
  Serial.print("\r");
  Serial.print("\n");

  Serial.println();
  //delay before next reading
  delay(1000);
}

//Use PuTTY to look at readings, make sure to use correct COM port and baud rate is 9600
