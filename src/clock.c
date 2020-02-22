#include "global.h"

static uint32_t current_sysclk_hz = 8000000;

static volatile uint8_t tim15_over = 0;

void set_sysclk_hsi() {
    RCC->CFGR &= ~(RCC_CFGR_SW);
    current_sysclk_hz = 8000000;
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
    current_sysclk_hz = 64000000;
}

void enable_timer(TIM_TypeDef *TIMx, uint8_t priority) {
    if (priority > 0xF)
        priority = 0xF;
    if (TIMx == TIM2) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
        NVIC_SetPriority(TIM2_IRQn, priority);
        NVIC_EnableIRQ(TIM2_IRQn);
    }
    else if (TIMx == TIM3) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
        NVIC_SetPriority(TIM3_IRQn, priority);
        NVIC_EnableIRQ(TIM3_IRQn);
    }
    else if (TIMx == TIM15) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
        NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, priority);
        NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
    }
    else if (TIMx == TIM16) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
        NVIC_SetPriority(TIM1_UP_TIM16_IRQn, priority);
        NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
    }
    else if (TIMx == TIM17) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
        NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, priority);
        NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
    }
}

void start_timer(TIM_TypeDef *TIMx, uint16_t periodms) {
    /* turn timer's counter off */
    TIMx->CR1 &= ~(TIM_CR1_CEN);
    /* reset */
    if (TIMx == TIM2) {
        RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
        RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);
    }
    else if (TIMx == TIM3) {
        RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
        RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM3RST);
    }
    else if (TIMx == TIM15) {
        RCC->APB2RSTR |= RCC_APB2RSTR_TIM15RST;
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM15RST);
    }
    else if (TIMx == TIM16) {
        RCC->APB2RSTR |= RCC_APB2RSTR_TIM16RST;
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM16RST);
    }
    else if (TIMx == TIM17) {
        RCC->APB2RSTR |= RCC_APB2RSTR_TIM17RST;
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM17RST);
    }

    /* set prescale and autoreload registers */
    TIMx->PSC = current_sysclk_hz / 1000;
    TIMx->ARR = periodms;
    /* reset timer */
    TIMx->EGR |= TIM_EGR_UG;
    /* enable interrupt */
    TIMx->DIER |= TIM_DIER_UIE;
    /* enable timer */
    TIMx->CR1 |= TIM_CR1_CEN;
}

void stop_timer(TIM_TypeDef *TIMx) {
    /* stop timer */
    TIMx->CR1 &= ~(TIM_CR1_CEN);
    /* clear interrupt flag */
    TIMx->SR &= ~(TIM_SR_UIF);
}

void TIM1_BRK_TIM15_IRQHandler() {
    if (TIM15->SR & TIM_SR_UIF) {
        /* clear interrupt flag */
        TIM15->SR &= ~(TIM_SR_UIF);
        /* set timer up flag */
        tim15_over = 1;
    }
}

void sleep(uint32_t msecs) {
    if (!(RCC->APB2ENR & RCC_APB2ENR_TIM15EN))
        enable_timer(TIM15, 1);
    start_timer(TIM15, msecs);
    while (!tim15_over)
        ;
    stop_timer(TIM15);
    tim15_over = 0;
}
