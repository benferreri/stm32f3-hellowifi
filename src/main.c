#include <stddef.h>
#include "global.h"
#include "uart.h"
#include "clock.h"

void sleep(uint32_t msecs) {
    #define STEPS_PER_MSEC 720
    uint32_t i,s;
    for (s=0; s < msecs; s++) {
        for (i = 0; i < STEPS_PER_MSEC; i++) {
            // skip CPU cycle
            asm("nop");
        }
    }
}

void flash_led(int msecs) {
        GPIOA->ODR |= (1 << LED_PIN);
        sleep(msecs);
        GPIOA->ODR &= ~(1 << LED_PIN);
}

void main() {

    /* set system clock to 64 MHz */
    set_sysclk_64();

    /* turn on GPIOA clock and setup LED pin modes
     * MODER 0x1 : general purpose output mode
     * OTYPER 0  : output push-pull 
     */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER  &= ~(0x3 << (LED_PIN*2));
    GPIOA->MODER  |=  (0x1 << (LED_PIN*2));
    GPIOA->OTYPER &= ~(1 << LED_PIN);

    flash_led(150);

    /* baud rate 115.2k, sysclk 64Mhz */
    enable_uart1(115200, 64000000);

    uint8_t message[] = "AT\r\n";
    size_t length = sizeof(message)/sizeof(message[0]) - 1;
    uart1_send(message);

    uint8_t s[100];
    while (1) {
        uint8_t c = uart1_getchar();
        if (c != 0)
            s[i++] = c;
    }
}
