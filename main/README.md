# Source Code
This folder `ElectroSwing/main` is the source code for Ava's swing system!

We have two arduinos. One for the timing, and one for signaling to the motor.
(Designated by primary.ino and secondary.ino)

We used the other files (outside of `primary` and `secondary` for testing an accelerometer, which, for the sake of time, we did not fully integrate into our system.

UMBC peers and other internal engineers, Please look at our System Design Document for an explanation of why we used different variables.

# Primary
The primary Arduino Uno Arduino’s is ATmega328P. 

Dependencies (Install these in the arduino IDE before running!):
- `LiquidCrystalDisplay_I2C.h`
- `TimerOne.h`

# Secondary
The secondary arduino is the Mega Rev3 Arduino whose chip architecture is atMega2560 with a raw clock speed of 16MHZ. We determined it was best to have the faster arduino, the Mega Rev3, be used for motor operations since trigonometry is necessary for our swing system. 


