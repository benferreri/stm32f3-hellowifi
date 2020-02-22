#include "global.h"

void set_sysclk_hsi() {
    RCC->CFGR &= ~(RCC_CFGR_SW);
}

void set_sysclk_64() {
    /* set PLL clock to 64 MHz and enable it
     * disable PLL: PLLON = 0
     * wait for PLLRDY
     * set PLL src to HSI/2
     * set PLL to HSI/2 * 16 = 64 MHz
     * enable PLL: PLLON = 1 
     */
    RCC->CR &= ~(RCC_CR_PLLON);
    while (RCC->CR & RCC_CR_PLLRDY)
        ;
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC);
    RCC->CFGR |= RCC_CFGR_PLLMUL16;
    RCC->CR |= RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY)
        ;

    /* set HCLK prescaler and APB2 prescaler to not divide
     * set APB1 prescaler to divide by 4
     */
    RCC->CFGR &= ~(0xF << 4) & ~(0x7 << 11) & ~(0x7 << 8);
    RCC->CFGR |= 0x5 << 8;

    /* increase FLASH latency (bits 2:0 to b010) */
    FLASH->ACR &= ~(0x7);
    FLASH->ACR |= 0x2 << 0;
    
    /* switch sysclk to PLL */
    RCC->CFGR &= (uint32_t) ~(RCC_CFGR_SW);
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while (!(RCC->CFGR &  (uint32_t)(RCC_CFGR_SW_PLL)))
        ;
}
