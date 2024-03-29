#ifndef MOTOR_ROTATOR_H
#define MOTOR_ROTATOR_H

// defines pins numbers
const int stepPin = 5;
const int dirPin = 2;
const int enPin = 8;

// INCREASE VALUE BY 50 TO GET A QUATER CIRCLE OF MOTION AT MOTOR STRENGTH 965
const int SPIN_TIME = 50; // CONTROLS HOW LONG MOTOR WILL SPIN FOR

// FOR MOTOR STRENGTH 350 MINIMUM
// 940 IS CONSISTENT WITH STARTING AND STOPPING IN THE SAME PLACE, WHEN ROTATING LEFT AND BACK TO RIGHT
// DONT GO ABOVE 965 FOR MOTOR STRENGTH
const int MOTOR_STRENGTH = 800; // THIS IN A DELAY VALUE IN MICROSECONDS, LONGER THE DEPLAY SLOWER THE MOTOR WILL BE

void motorRotatorSetup();
void motorRotatorLoop();
    
#endif
