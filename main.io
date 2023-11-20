

#include<time.h>

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




//pins for decimal point and each segment
//dp, G, F, E, D, C, B, A
const short int segmentPins[]= { 1, 2, 3, 4, 5, 7, 10, 11};///used for 7 segment
const short int buttonPins[] = {incrementPin,decrementPin,startPin,stopPin};;//used for buttons

const short int numberofDigits=4;

const short int digitPins[numberofDigits] = { 6,8,9, 12}; //digits 1, 2, 3, 4


//setup button pins
void buttonSetup(){
  for (int i=0; i < 4; i++){
    pinMode(buttonPins[i], INPUT); //set segment and DP pins to output
  }

}



//FUCTION SETUPS CLOCK INTERRUPTS
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
  TIMSK1 |= (1 << OCIE1A);
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

  while(digitalRead(pinNumber) == LOW){
    //DO NOTHING BUT DISPLAY TIME
    displayTime();//display current selected time
    //stay in this while loop until the pin in question goes high again
  }

}


void getSwingTime(){

  while(digitalRead(startPin) == HIGH){//be in this "SET TIME" MODE while start pin is low


    //IF THE THE STOP BUTTON IS PRESSED
    if(digitalRead(stopPin) != HIGH){//clear time
      //debounceButton(stopPin);//wait until stopPin is let go
      clearTimer();//clear time
    }

    //IF THE INCREMENT BUTTON IS PRESSED
    else if(digitalRead(incrementPin) != HIGH){

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
    else if(digitalRead(decrementPin) != HIGH){

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

      if(digitalRead(stopPin) == LOW){//IF STOP BUTTON PRESSED, CLEAR TIMER AND END SWING
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



void setup()
{

 
  //USED FOR TESTING
  /*
  minTens = 1;//tens place for minutes
  minOnes = 3;//ones place for minutes
  secTens = 1;//tens place for seconds
  secOnes = 2;//ones place for seconds
  */

  

  buttonSetup();//.setup button pins
  segmentPinSetup();//setup 7 segment pins
  getSwingTime();//get swing time from opereator via physical timer interface
  clockInterruptSetup();//setup clock interrupts
  motorRotatorSetup();//setup motors
}

//MOTOR WILL KEEP OSCILLATING SO LONG AS THERE'S TIME REMAINING
//ONCE TIME RUNS OUT THE SEVEN SEG SHOULD DISPLAY 00:00 FOREVER
void loop(){
  displayTime();
  //motorRotatorLoop();//move motors

  if(secOnes != 0 || secTens != 0 && minOnes != 0 || minTens != 0){
      motorRotatorLoop();//move motors
  }
}

//With the settings above, this IRS will trigger each 500ms.
ISR(TIMER1_COMPA_vect){
    decrementTimer();//DECREMENT TIMER BY A SECOND
    //displayTime();

}
