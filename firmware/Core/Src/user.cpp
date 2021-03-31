#include "main.h"

extern "C" void user_main()
{
    TIM3->CCR1 = 2500;
    TIM3->CCR2 = 5000;
    TIM3->CCR3 = 5000;
    TIM3->CCR4 = 5000;
    TIM3->CCER |= LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH4;
    TIM3->BDTR |= TIM_BDTR_MOE;
    TIM3->CR1 |= TIM_CR1_CEN;
    
    GPIOA->BSRR = 1<<10 | 1<<11 | 1<<12;
    
    TIM14->ARR = 20000;
    TIM14->CCR1 = 10000;
    TIM14->CCER |= LL_TIM_CHANNEL_CH1;
    TIM14->BDTR |= TIM_BDTR_MOE;
    TIM14->CR1 |= TIM_CR1_CEN;

    while(true) {
    }
}
