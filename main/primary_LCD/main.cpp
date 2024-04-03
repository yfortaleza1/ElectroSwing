
#include <iostream>
#include <cmath>
#include "timer_variables.h"

using namespace std;

void updateDigitsFromTotalSeconds(int totalSeconds) {
    /*
    Change our variables which correspond to each digit we display for the time
    based on the given number of seconds.
    */
    // cli();
    int totalMinutes = floor(totalSeconds / SECONDS_PER_MINUTE);
    int leftoverSeconds = totalSeconds % SECONDS_PER_MINUTE;

    // reassign our global variales for the digits
    // example 300 seconds should be 05:00
    minTens = floor(totalMinutes / MAX_TENS_DIGIT + 1) ;
    minOnes = totalMinutes % (MAX_ONES_DIGIT + 1);
    secTens = floor(leftoverSeconds / MAX_TENS_DIGIT + 1) ;
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
            (SECONDS_PER_MINUTE*10) * secTens);
}

// copy and pasting code to test it.
void incrementTimer(){

    int nextTotalSeconds = calculateTotalSeconds();
    nextTotalSeconds += SECONDS_INCREMENT;

    updateDigitsFromTotalSeconds(nextTotalSeconds);

}

int main() {
    incrementTimer();
    cout << minTens << minOnes << ":" << secTens << secOnes << endl;
    return 0;
}