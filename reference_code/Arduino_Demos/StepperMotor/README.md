# Where this code comes from
Sample code taken from the following link:
https://docs.arduino.cc/learn/electronics/stepper-motors

Here are the descriptions provided 
on that link for each sample code:
(I've also pasted these as the header comment of each file)

## MotorKnob
"A stepper motor 
follows the turns of a potentiometer 
(or other sensor) 
on analog input 0."

# StepperOneRevolution
"The motor should revolve 
one revolution in one direction, 
then one revolution in the other direction."

# StepperOneStepAtATime
"The motor will step 
one step at a time, very slowly. 

You can use this to test 
that you've got the four wires 
of your stepper wired to the correctpins. 
If wired correctly, 
all steps should be in the same direction."

*^ seems like this 
will be useful for testing 
on our 11/3 workday!*

# StepperSpeedControl
"The motor will rotate in a clockwise direction. 
The higher the potentiometer value, 
the faster the motor speed. 

Because setSpeed() sets the delay between steps, 
you may notice 
the motor is less responsive 
to changes in the sensor value at low speeds."