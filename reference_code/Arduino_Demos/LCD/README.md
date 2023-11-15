Code in this folder comes from the following arduino documentation
https://docs.arduino.cc/learn/electronics/lcd-displays

# Notes
## Libraries
### LiquidCrystal
https://www.arduino.cc/reference/en/libraries/liquidcrystal/
```{cpp}
#include <LiquidCrystal.h>
```
- install through Library Manager in Arduino IDE
## Pins
### register select (RS) pin
- = destination of our write
- options for RS values
    - data register
    - = holds what is displayed
    - instruction register
    - = instrs for what to do next
### Read/Write (R/W) pin
- designate whether reading or writing mode
### Enable pin
- allows you to write to the registers
### Data pins (D0-D7)
- states: high/low
- holds the intermediate values from write/read
### Display contrast pin (V0)
### Power Supply Pins
- +5V
- GND
### LED Backlight
- Bklt+
- BKlt- 

## Hardware Required
- Arduino Board
- LCD Screen (compatible with Hitachi HD44780 driver)
- pin headers to solder to the LCD display pins
- 10k ohm potentiometer
- 220 ohm resistor
- hook-up wires
- breadboard

## Circuit setup
To wire your LCD screen to your board, connect the following pins:

LCD RS pin to digital pin 12
LCD Enable pin to digital pin 11
LCD D4 pin to digital pin 5
LCD D5 pin to digital pin 4
LCD D6 pin to digital pin 3
LCD D7 pin to digital pin 2
LCD R/W pin to GND
LCD VSS pin to GND
LCD VCC pin to 5V
LCD LED+ to 5V through a 220 ohm resistor
LCD LED- to GND
