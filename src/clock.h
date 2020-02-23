#ifndef __CLOCK_H
#define __CLOCK_H

void set_sysclk_hsi();
void set_sysclk_64();

void sleep(uint32_t msecs);

void TIM1_BRK_TIM15_IRQHandler();

#endif
