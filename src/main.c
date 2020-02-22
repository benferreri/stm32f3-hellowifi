#include <stddef.h>
#include "global.h"
#include "uart.h"
#include "clock.h"
#include "ustring.h"
#include "wifi.h"

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

    uint8_t message[5] = "AT\r\n";
    uint8_t response[8];
    sendcmd((uint8_t*)"AT\r\n", response, 8);
    if (ustrcmp(response, (uint8_t*)"OK") == 0)
        while (1) {
            flash_led(1000);
            sleep(1000);
        }
    else
        while (1) {
            flash_led(3000);
            sleep(3000);
        }
}
