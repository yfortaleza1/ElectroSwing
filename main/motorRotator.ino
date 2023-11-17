/*
* Project: Ava's Motorized Swing
* File:    motorRotator.ino
* Authors: Marc Conn, Alec Kalish
* Date:    Tues. 11/14/2023
*/
#include "motorRotator.h"

// code within these functions are still the same, 
// now we should be able to call the setup within main.ino, 
// where we will also tie in the timer setup and loop. 
void setup()
{
    motorRotatorSetup();
}
void loop()
{
    motorRotatorLoop();
}

void motorRotatorSetup() {
    // Sets the two pins as Outputs
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, LOW);

}

void motorRotatorLoop() {
    digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction
    for (int x = 0; x < SPIN_TIME; x++)
    { // SPIN_TIME VALUE DICTATES HOW LONG MOTOR WILL SPIN
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(MOTOR_STRENGTH);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(MOTOR_STRENGTH);
    }

    // THIS DELAY IS WEIRD, BUT NEED TO BE INCLUDED OR ELSE THE CODE WONT WORK
    // REASON UNCLEAR
    delay(10);                 // THIS NEEDS TO BE HERE
    digitalWrite(dirPin, LOW); // Changes the rotations direction

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
}