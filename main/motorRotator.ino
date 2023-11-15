
// put your setup code here, to run once:
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

void setup()
{

    // Sets the two pins as Outputs
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, LOW);
}
void loop()
{

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
