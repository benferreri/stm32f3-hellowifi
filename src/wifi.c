#include "global.h"
#include "uart.h"
#include "clock.h"

void sendcmd(uint8_t command[], uint8_t response[], uint32_t responselen) {
    uint8_t c;
    uint32_t i = 0;
    uint32_t j = 1;
    uart1_send(command);
    sleep(500);
    while ((c = uart1_getchar()) == '\r' || c == '\n')
        ; // skip
    while ((c = uart1_getchar()) == command[j++] && c != '\r' && c != '\n')
        ; // esp8266 echoes the command
    while ((c = uart1_getchar()) == '\r' || c == '\n')
        ; // skip
    /* copy response */
    response[i++] = c;
    while (i < responselen - 1 && (c = uart1_getchar()) != '\r' && c != '\n')
        response[i++] = c;
    response[i] = '\0';
}
