# hello wifi

A simple bare-metal (no HAL/LL) example of communication between an STM32F334R8 and an ESP8266 (specifically an ESP-01 in my case) over UART.

Compile with `$ make`

Remove unnecessary files with `$ make clean` (some of these files are necessary for debugging with `gdb`)

Flash with `$ make flash`
