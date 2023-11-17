

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


//pins for decimal point and each segment
//dp, G, F, E, D, C, B, A
const short int segmentPins[]= { 1, 2, 3, 4, 5, 7, 10, 11};

const short int numberofDigits=4;

const short int digitPins[numberofDigits] = { 6,8,9, 12}; //digits 1, 2, 3, 4

//FUCTION SETUPS CLOCK INTERRUPTS
void clockInterruptSetup(){

  cli();                      //stop interrupts for till we make the settings
  /*1. First we reset the control register to amke sure we start with everything disabled.*/
  TCCR1A = 0;                 // Reset entire TCCR1A to 0 
  TCCR1B = 0;                 // Reset entire TCCR1B to 0
 
  /*2. We set the prescalar to the desired value by changing the CS10 CS12 and CS12 bits. */  
  TCCR1B |= B00000100;        //Set CS12 to 1 so we get prescalar 256  
  
  /*3. We enable compare match mode on register A*/
  TIMSK1 |= B00000010;        //Set OCIE1A to 1 so we enable compare match A 
  
  /*4. Set the value of register A to 31250*/
  //DECREASE OCR1A FROM 31250 TO MAKE A SECOND OCCUR FASTER
  OCR1A = 31250/4;             //Finally we set compare register A to this value  
  sei(); //turn all interrupts back on


}

//FUNCTION STEPS UP I/O PINS
void pinSetup(){
  for (int i=0; i < 8; i++){
    pinMode(segmentPins[i], OUTPUT); //set segment and DP pins to output
  }
  //sets the digit pins as outputs
  for (int i=0; i < numberofDigits; i++){
    pinMode(digitPins[i], OUTPUT);
  }


}



void timing(){


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

  if(secOnes == 0 && secTens == 0 && minOnes == 0 && minTens == 0){
    delay(10);//THIS WILL CAUSE LEDS TO VISIBLY FLICKER, MAY BE ANNOYING :(
  }
  
}



void setup()
{

 
  minTens = 1;//tens place for minutes
  minOnes = 3;//ones place for minutes
  secTens = 1;//tens place for seconds
  secOnes = 2;//ones place for seconds

  

  
  pinSetup();//setup pins
  clockInterruptSetup();//setup clock interrupts
}

void loop(){
  //timing();
  showDigit (minTens, 4);
  showDigit (minOnes, 3);
  showDigit (secTens, 2);
  showDigit (secOnes, 1);
  //showDigit(9,4);
  //digitalWrite(5, HIGH);//selects digit to have number displayed on
  //digitalWrite(11, HIGH);
  //delay(1000);
}

//With the settings above, this IRS will trigger each 500ms.
ISR(TIMER1_COMPA_vect){
  TCNT1  = 0;                  //First, set the timer back to 0 so it resets for next interrupt
  secTicks +=1;

  if(secTicks != 0 && secTicks % 2 == 0){
    timing();
    secTicks = 0;
  }
  
}