#include <stddef.h>
#include "global.h"
#include "clock.h"
#include "uart.h"

static uint8_t uart1_enabled = 0;

/* rx buffer for uart1 */
static struct {
    uint8_t buffer[RX_BUFFER_SIZE];
    uint8_t *readptr;
    uint8_t *writeptr;
    uint8_t full;
    uint8_t overrun;
} rxbuf1;

/* static function declarations */
static uint32_t uart1_tx_ready();
static uint32_t uart1_tx_complete();
static uint32_t uart1_rx_ready();
static void uart1_writetobuf();

/* enables uart1 on pins PA9 (TX) and PA10 (RX) 
 * baud_rate is baud rate for uart communication
 * clk_speed is the speed of sysclk
 * buffer_size is the size of the RX buffer (stored in RAM, max 1k)
 */
void enable_uart1(uint32_t baud_rate, uint32_t clk_speed) {

    /* set usart1 clock source to be SYSCLK */
    RCC->CFGR3 &= ~(RCC_CFGR3_USART1SW);
    RCC->CFGR3 |= RCC_CFGR3_USART1SW_SYSCLK;
    
    /* enable usart1 clock and GPIOA clock */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;

    /* set pins to alt function mode and high speed
     * alt function mode: b10
     * high speed mode:   b11
     */
    GPIOA->MODER &= ~(0x3 << 2 * TX1_PIN);
    GPIOA->MODER |=  (0x2 << 2 * TX1_PIN);
    GPIOA->MODER &= ~(0x3 << 2 * RX1_PIN);
    GPIOA->MODER |=  (0x2 << 2 * RX1_PIN);
    GPIOA->OSPEEDR |= (0x3 << 2 * RX1_PIN);
    GPIOA->OSPEEDR |= (0x3 << 2 * TX1_PIN);
    
    /* set alt function to 7 (USART) */
    GPIOA->AFR[1] &= ~(0xF << 4 * (RX1_PIN - 8));
    GPIOA->AFR[1] |=  (0x7 << 4 * (RX1_PIN - 8));
    GPIOA->AFR[1] &= ~(0xF << 4 * (TX1_PIN - 8));
    GPIOA->AFR[1] |=  (0x7 << 4 * (TX1_PIN - 8));
    
    /* set word length to 8 data bits */
    USART1->CR1 &= ~(USART_CR1_M0);
    USART1->CR1 &= ~(USART_CR1_M1);

    /* set oversampling mode to 8 */
    USART1->CR1 |= USART_CR1_OVER8;

    /* set baud rate 
     * for OVER8=1 :
     * BRR bits [15:4] = usartdiv bits [15:4]
     * BRR bit 3 is cleared
     * BRR bits [2:0] = usartdiv bits [3:1] shifted right by 1
     */
    USART1->BRR &= ~(0xFFFF);
    uint16_t usartdiv = 2 * clk_speed / baud_rate;
    USART1->BRR |= (0x7 & (usartdiv >> 1)) | (usartdiv & ~(0xF));

    /* set number of stop bits to 1 (00) */
    USART1->CR2 &= ~(USART_CR2_STOP);

    /* initialize rx buffer */
    rxbuf1.readptr = rxbuf1.writeptr = &rxbuf1.buffer[0];   
    rxbuf1.full = 0;
    rxbuf1.overrun = 0;
    
    /* enable rx/tx */
    USART1->CR1 |= USART_CR1_TE;
    USART1->CR1 |= USART_CR1_RE;

    /* enable uart */
    USART1->CR1 |= USART_CR1_UE;
    uart1_enabled = 1;
    
    /* turn on rxne interrupt (0xF = highest priority) */
    USART1->CR1 |= USART_CR1_RXNEIE;
    NVIC_SetPriority(USART1_IRQn, 0xF);
    NVIC_EnableIRQ(USART1_IRQn);
}

/* check if uart1 is enabled and if transmit data register is empty */
uint32_t uart1_tx_ready() {
    return uart1_enabled && (USART1->ISR & USART_ISR_TXE);
}

/* check if transmission is complete */
uint32_t uart1_tx_complete() {
    return USART1->ISR & USART_ISR_TC;
}

/* send data over uart1.
 * returns 0 on success.
 * returns > 0 if transmit data register not empty 
 */
uint32_t uart1_send(uint8_t message[]) {
    if (!uart1_tx_ready())
        return 1;
    uint32_t i = 0;
    while ((c = message[i++]) != '\0') {
        USART1->TDR = c;                // send char
        while (!uart1_tx_ready())
            ;
    }
    return 0;
}

/* Generic Interrupt Handler */
void USART1_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_RXNE) {
        uart1_writetobuf();
    }
}

/* handles interrupts for RX data register ready to be read.
 * writes character to circular buffer (rxbuf1).
 * if buffer out of space, sets flag and does not read from
 * register, which may lead to overrun error.
 */

static void uart1_writetobuf() {
    /* if at end of array, wrap around */
    if (rxbuf1.writeptr == &rxbuf1.buffer[0] + RX_BUFFER_SIZE)
        rxbuf1.writeptr = &rxbuf1.buffer[0];
    /* read and advance write pointer.
     * if catching up to read pointer, don't advance write and mark full.
     * if full, don't write (may lead to overrun error)
     */
    if (rxbuf1.writeptr != rxbuf1.readptr - 1)
        *rxbuf1.writeptr++ = 0xFF & USART1->RDR;
    else if (!rxbuf1.full) {
        *rxbuf1.writeptr = 0xFF & USART1->RDR;
        rxbuf1.full = 1;
    }
    else
        rxbuf1.overrun = 1;
}

/* check if uart1 is enabled and if read data register is not empty */
static uint32_t uart1_rx_ready() {
    return uart1_enabled && (USART1->ISR & USART_ISR_RXNE);
}

/* get a received character from the buffer.
 * returns 0 if the buffer is empty.
 */
uint8_t uart1_getchar() {
    uint8_t c;
    if (rxbuf1.readptr != rxbuf1.writeptr) {
        c = *rxbuf1.readptr++;
        if (rxbuf1.full) {
            rxbuf1.full = 0;
            if (rxbuf1.overrun) {
                uart1_writetobuf();
                rxbuf1.overrun = 0;
            }
        }
    }
    else
        c = 0;
    return c;
}
