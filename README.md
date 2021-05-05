# ArduinoClockDiscovery
Arduino Clockspeed Discovery Tool

A simple sketch to figure out the actual frequency an arduino is trying to run at.

Useful for cheap micro/nano boards that don't say on the silkscreen and use a tiny crystal that has no writing on it, or too small writing

## How to use:

* Use a programmer that allows Serial to a terminal / serial monitor on your PC
* Set baud on serial monitor at 9600
* Upload the sketch to the Arudino
* The serial string will be transmitted at 2400, 4800, 9600, 19200 and 38400
* Clock information will appear on the serial port, along with junk data due to the other baud rates being mismatched.
