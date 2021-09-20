#!/bin/sh

# Loads the binary into the board's flash.

openocd -f interface/picoprobe.cfg -f target/rp2040.cfg \
-c "program build/moisture.elf verify reset exit"
