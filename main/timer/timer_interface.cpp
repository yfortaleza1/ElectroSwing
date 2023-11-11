// code for displaying ASCII from https://www.scaler.com/topics/cpp-timer/
// todo: include increment, start, stop using IO
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "timer_interface.h"

using namespace std;

void displayClock() {
    cout << "        TIMER      \n";
    cout << "-------------------\n";

    cout << "| " << setfill('0') ; 

    cout << setfill('0') << setw(2) << minutes << " min | ";

    cout << setfill('0') << setw(2) << seconds << " sec |" << endl;

    cout << " ------------------\n";
}

void timer() {

    // Infinite while loop.
    while (true) {
        displayClock();
        sleep(1);
        seconds++;

        if (seconds == 60) {
            minutes++;

            if (minutes == 60) {
                minutes = 0;
            }

            seconds = 0;
        }
    }
}

int main() {
    timer();
    return 0;
}