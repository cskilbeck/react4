#include "main.h"
#include "util.h"

extern "C" volatile unsigned long ticks = 0;

struct note
{
    uint16 timer;
    uint16 delay;
};

note charge[] = {
    { 15306	,125  } ,
    { 11467	,125  } ,
    { 9101	,125  } ,
    { 7653	,300  } ,
    { 9101	,200  } ,
    { 7653	,700  } ,
    { 1     ,1000 }
};

int charge_len = countof(charge);

extern "C" void user_main()
{
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    NVIC_EnableIRQ(SysTick_IRQn);
    TIM3->CCR1 = 4500;
    TIM3->CCR2 = 4000;
    TIM3->CCR3 = 3500;
    TIM3->CCR4 = 3000;
    TIM3->CCER |= LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH4;
    TIM3->BDTR |= TIM_BDTR_MOE;
    TIM3->CR1 |= TIM_CR1_CEN;
    
    GPIOA->BSRR = 1 << 8 | 1<<10 | 1 << 11 | 1 << 12;
    
    TIM14->CCER |= LL_TIM_CHANNEL_CH1;
    TIM14->BDTR |= TIM_BDTR_MOE;
    TIM14->CR1 |= TIM_CR1_CEN;
    
    int note_index = -1;
    int delay = 0;
    int then = ticks;
    while(true) {
        int tk = ticks;
        int elapsed = tk - then;
        if(elapsed >= delay) {
            then = tk;
            note_index += 1;
            if(note_index >= charge_len) {
                note_index = charge_len - 1;
            }
            int t = charge[note_index].timer;
            TIM14->ARR = t;
            TIM14->CCR1 = t / 2;
            delay = charge[note_index].delay;
        }
    }
}
