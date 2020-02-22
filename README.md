# hello wifi

A simple bare-metal (no HAL/LL) example of communication between an STM32F334R8 and an ESP8266 (specifically an ESP-01 in my case) over UART.

Compile with `$ make`

Remove unnecessary files with `$ make clean` (some of these files are necessary for debugging with `gdb`)

Flash with `$ make flash`

This code was developed for an STM32F334R8, and it will likely work for any STM32F3 (just replace `device_headers/stm32f334x8.h` with the appropriate header file).
