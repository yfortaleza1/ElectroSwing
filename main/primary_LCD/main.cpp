
#include <iostream>
#include <cmath>
#include "timer_variables.h"

using namespace std;

#define DEBUG 1

// copy and pasting code to test it.
void updateDigitsFromTotalSeconds(int totalSeconds) {
    /*
    Change our variables which correspond to each digit we display for the time
    based on the given number of seconds.
    */
    // cli();
    int totalMinutes = floor(totalSeconds / SECONDS_PER_MINUTE);
    int leftoverSeconds = totalSeconds % SECONDS_PER_MINUTE;
    if (DEBUG) {
        cout << "LEFTOVER SECONDS: " << leftoverSeconds << endl;
    }

    // reassign our global variales for the digits
    // example 300 seconds should be 05:00
    minTens = floor(totalMinutes / (MAX_TENS_DIGIT + 1)) ;
    minOnes = totalMinutes % (MAX_ONES_DIGIT + 1);
    secTens = floor(leftoverSeconds / (MAX_TENS_DIGIT + 1)) ;
    secOnes = leftoverSeconds % (MAX_ONES_DIGIT + 1);

    if(minOnes == 9 && minTens != 5){//ONES PLACE OVERFLOW CONDITION
      minOnes = 0;//set ones place of mines to 0
      minTens += 1;//incrmeent tens place of minutes
    }

    // sei();
}

int calculateTotalSeconds() {
    // time in seconds!

    return (secOnes + 10 * secTens + // add up the seconds
            // add up the minutes
            SECONDS_PER_MINUTE * minOnes + 
            (SECONDS_PER_MINUTE*10) * minTens);
}



void clearTimer(){
  secOnes = 0;
  secTens = 0;
  minOnes = 0;
  minTens = 0;
}

void printTimeToCout() {
    cout << minTens << minOnes << ":" << secTens << secOnes << endl;
}



void incrementTimer(){

    int nextTotalSeconds = calculateTotalSeconds();
    if (DEBUG == 1) {
        cout << "DEBUG icrementTimer()" << endl;
        cout << "Initial totalSeconds " << nextTotalSeconds << endl;
    }
    nextTotalSeconds += SECONDS_INCREMENT;
    if (DEBUG == 1) {
        cout << "After totalSeconds " << nextTotalSeconds << endl;
        printTimeToCout();
    }
    updateDigitsFromTotalSeconds(nextTotalSeconds);
     if (DEBUG == 1) {
        cout << "After updateDigits() " << endl;
        printTimeToCout();
    }

}

int main() {
    clearTimer();
    printTimeToCout();
    incrementTimer();
    printTimeToCout();
    return 0;
}