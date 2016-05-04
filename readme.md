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
a Raspberry Pi 3.

For further information about this project see Element 14.

