const int sdaPin = A4; //analog input 4 -- ground
const int sdlPin = A5; //analog input 5 -- voltage
const int xPin = A3; //x-axis
const int yPin = A2; //y-axis
const int zPin = A1; //z-axis

/*
* Displays accelerometer reading at every given time interval 
*/
void getAccel(int time){
  // put your main code here, to run repeatedly:

  //print the sensor values:
  Serial.print("x-axis: ");
  Serial.println(analogRead(xPin));
  Serial.print("\r");
  Serial.print("\n");

  Serial.print("y-axis: ");
  Serial.println(analogRead(yPin));
  Serial.print("\r");
  Serial.print("\n");

  Serial.print("z-axis: ");
  Serial.println(analogRead(zPin));
  Serial.print("\r");
  Serial.print("\n");

  Serial.println();
  //delay before next reading
  delay(time);
}

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

  getAccel(1000);

}



