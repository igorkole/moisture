Raspberry Pi Pico Learning Project

Components in use

The Raspberry Pi Pico microcontroller board
https://www.raspberrypi.org/products/raspberry-pi-pico/

Sparkfun Soil Moisture Sensor
https://learn.sparkfun.com/tutorials/soil-moisture-sensor-hookup-guide

Adafruit NeoPixel Ring
https://www.adafruit.com/product/2852

[A schematic of the device under test](schematic.pdf)

Software

Raspberry Pi Pico SDK
https://github.com/raspberrypi/pico-sdk

moisture.c is a program that reads the sensor's data and writes the data
to the serial output.  The output is directed to UART0 and USB serial.
