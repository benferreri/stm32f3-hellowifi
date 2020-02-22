#ifndef __UART_H
#define __UART_H

#define TX1_PIN 9
#define RX1_PIN 10
#define RX_BUFFER_SIZE 1024

void enable_uart1(uint32_t baud, uint32_t clk_speed);

uint32_t uart1_send(uint8_t message[], size_t length);
uint8_t  uart1_getchar();

void USART1_IRQHandler(void);

#endif
