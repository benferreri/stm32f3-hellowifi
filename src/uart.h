#ifndef __UART_H
#define __UART_H

void enable_uart1(uint32_t baud, uint32_t clk_speed);

uint32_t uart1_send(uint8_t message[], size_t length);
uint8_t  uart1_getchar();

void USART1_IRQHandler(void);

#endif
