void setup() {
  // put your setup code here, to run once:
// defines pins numbers
const int stepPin = 5; 
const int dirPin = 2; 
const int enPin = 8;
const int SPIN_TIME = 1000;//CONTROLS HOW LONG MOTOR WILL SPIN FOR
const int MOTOR_STRENGTH = 500;//THIS IN A DELAY VALUE IN MICROSECONDS, LONGER THE DEPLAY SLOWER THE MOTOR WILL BE
void setup() {
  
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);

  pinMode(enPin,OUTPUT);
  digitalWrite(enPin,LOW);
  
}
void loop() {
  
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
// Makes 200 pulses for making one full cycle rotation
 for(int x = 0; x < SPIN_TIME; x++) {
   digitalWrite(stepPin,HIGH); 
   delayMicroseconds(MOTOR_STRENGTH); 
   digitalWrite(stepPin,LOW); 
   delayMicroseconds(MOTOR_STRENGTH); 
  }
  delay(10);
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  delay(2000); // One second delay
  
   //Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < SPIN_TIME; x++) {//
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(MOTOR_STRENGTH);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(MOTOR_STRENGTH);
  }
  delay(10);
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  delay(2000);


}

}

void loop() {
  // put your main code here, to run repeatedly:

}