Keysight U1282A FastLog
-----------------------

Fastlog is a software application that captures data from Keysight
U1282A multimeters using the standard IR-USB adapter supplied with them.
The software may work with other Keysight multimeters too.

To compile, use
gcc fastlog.c -o fastlog

Example usage:
fastlog                  - dumps 10 results to the display
fastlog 100              - dumps 100 results to the display
fastlog -t /dev/ttyUSB0  - use /dev/ttyUSB0 (this is the default)
fastlog -o log.csv       - write to CSV file (as well as dumping to display)

This software should work on all Linux platforms; it was tested on
a Raspberry Pi 3 and on Ubuntu running on a x86 PC.

fastlog.m is a MATLAB program with similar functionality in that it can retrieve
data from the U1282A, but it directly plots it to the screen. To use it, just type
fastlog at the MATLAB command window.

For further information about this project see Element 14.
https://www.element14.com/community/groups/test-and-measurement/blog/2016/05/05/keysight-u1282a-data-logger-software



