#ifndef TIMER_VARIABLES_H
#define TIMER_VARIABLES_H

#define SECONDS_INCREMENT 30
#define SECONDS_PER_MINUTE 60

#define MAX_TENS_DIGIT 5
#define MAX_ONES_DIGIT 9

short int secTicks = 0;//used to keep track when a second should occur, clock is 16MHZ, so count up to 16 mill and decremenet timer
short int minTens = 0;//tens place for minutes
short int minOnes = 0;//ones place for minutes
short int secTens = 0;//tens place for seconds
short int secOnes = 0;//ones place for seconds

#endif
