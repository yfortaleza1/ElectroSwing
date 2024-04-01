# Source Code
This folder `ElectroSwing/main` is the source code for Ava's swing system!

We have two arduinos. One for the timing, and one for signaling to the motor.
(Designated by primary.ino and secondary.ino)

# Secondary
@mconn1 Feel free to fill this in to describe secondary.ino

# Primary

Currently the LCD code is in `./primary_LCD/primary_LCD.ino`
Once we reach a consensus this will all be combined in `primary.ino`

Jess:
- button response (start, stop, increment, decrement)
    - create a constant for how much to incr/decr by.
    - flag for isTimerStarted

Yoel:
- hardware respond to frequencies and display it on the LCD.
