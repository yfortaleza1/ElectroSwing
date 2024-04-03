/*
Name: primary.ino
Authors: Marc Conn
(soon to be contributers - Yoel Fortaleza, Jess Turner)
Date: 3/27 (planning on changing this current code to be LCD display rather than seven segment)
Description: 
This file will control the timing for the swing system.
Todo: use master pin to signal to secondary that it can do its logic. 
(Meaning that ride time is not zero, so it should continue swinging.)
*/

//THIS IS USING ATMEGA2560 
//ONLY WORKS FOR THE MEGA REV3, THE ONE WE ARE PLANNING ON USING FOR THE FINAL PRODUCT
//THIS CODE WON'T WORK ON THE UNO BECCAUSE OF DIFFERENT REGISTERS AND CLOCK SPEED (NEED TO CONFIRM)

const int sensorPin= 0;//The analog sensor is connected to analog pin 0 of the arduino

//ASSUMING THE CLOCK IS 8MHZ

short int secTicks = 0;//used to keep track when a second should occur, clock is 16MHZ, so count up to 16 mill and decremenet timer
short int minTens = 0;//tens place for minutes
short int minOnes = 0;//ones place for minutes
short int secTens = 0;//tens place for seconds
short int secOnes = 0;//ones place for seconds

//BUTTON PINs
const short int incrementPin = 22;//used for incrementing time
const short int decrementPin = 23;//used for decremnting time


//pins for decimal point and each segment
//dp, G, F, E, D, C, B, A
const short int segmentPins[]= { 2, 3, 4, 5, 6, 8, 11, 12};///used for 7 segment
const short int buttonPins[] = {incrementPin,decrementPin,startPin,stopPin};;//used for buttons

const short int numberofDigits=4;

const short int digitPins[numberofDigits] = { 7,9,10,13}; //digits 1, 2, 3, 4



//setup button pins
void buttonSetup(){
  for (int i=0; i < 4; i++){
    pinMode(buttonPins[i], INPUT_PULLUP); //set segment and DP pins to output
  }

}



//USED THIS FOR TRIGGERING SEGMENT DISPLAY
//THIS IS USING TIMER0
//16 bit timer
void SevenSegTimerSetup() {
  cli();
  // Clear registers
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 0;

  // 100.16025641025641 Hz (16000000/((155+1)*1024))
  //period is 0.01 seconds
  //OCR0A = 155;

  // 744.047 Hz (16000000/((20+1)*1024))
  //Period is 0.001344 seconds
  OCR0A = 30;
  // CTC
  TCCR0A |= (1 << WGM01);
  // Prescaler 1024
  TCCR0B |= (1 << CS02) | (1 << CS00);
  // Output Compare Match A Interrupt Enable
  TIMSK0 |= (1 << OCIE0A);
  sei();
}



//USED FOR CLEARING SWING TIME
void clearTimer(){
  secOnes = 0;
  secTens = 0;
  minOnes = 0;
  minTens = 0;
}



//this function is responsible for time decrementing logic
//Function decrements timer by 1 SECOND
//THIS FUNCTION WILL BE CALLED BY ISR EVERY SECOND TO GET THE TIMING RIGHT
void decrementTimer(){
      cli();
      if(digitalRead(stopPin) != LOW){//IF STOP BUTTON PRESSED, CLEAR TIMER AND END SWING
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

      sei();
}






void incrementTimer(){

      cli();
      if(minOnes < 9 && minOnes >= 0){
        minOnes +=1;//increment minsOnes place
      }

      else if(minOnes == 9 && minTens != 5){//ONES PLACE OVERFLOW CONDITION
        minOnes = 0;//set ones place of mines to 0
        minTens += 1;//incrmeent tens place of minutes
      }

      sei();
}

void showTime(){
    
    Serial.print(minTens);
    Serial.print(minOnes);
    Serial.print(":");
    Serial.print(secTens);
    Serial.print(secOnes);


    lcd.setCursor(14,3);
    lcd.print(minTens);
    lcd.print(minOnes);
    lcd.print(":");
    lcd.print(secTens);
    lcd.print(secOnes);
}

void setup()
{

 
  //USED FOR TESTING
  
  minTens = 1;//tens place for minutes
  minOnes = 3;//ones place for minutes
  secTens = 1;//tens place for seconds
  secOnes = 2;//ones place for seconds
  
  //cli();
  attachInterrupt(digitalPinToInterrupt(incrementPin),incrementTimer, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(decrementPin),decrementTimer, CHANGE); 

  //SevenSegTimerSetup();
  //sei();
   Serial.begin(9600);

}





//MOTOR WILL KEEP OSCILLATING SO LONG AS THERE'S TIME REMAINING
//ONCE TIME RUNS OUT THE SEVEN SEG SHOULD DISPLAY 00:00 FOREVER
void loop(){


  Serial.println("HEY");




}







