#include "main.h"
#include "util.h"

volatile unsigned long ticks = 0;

struct note
{
    uint16 timer;
    uint16 delay;
};

note charge[] = {
    { 15306	,100  } ,
    { 11467	,100  } ,
    { 9101	,100  } ,
    { 7653	,200  } ,
    { 9101	,100  } ,
    { 7653	,400  } ,
    { 1     ,1000 }
};

int charge_len = countof(charge);

uint32 get_buttons()
{
    uint32 b = GPIOA->IDR & 0xf;
    b = (b >> 1) | ((b & 1) << 3);
    return b;
}

extern "C" void on_systick()
{
    ticks += 1;
}

extern "C" void user_main()
{
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    NVIC_EnableIRQ(SysTick_IRQn);
    TIM3->CCR1 = 00;
    TIM3->CCR2 = 00;
    TIM3->CCR3 = 00;
    TIM3->CCR4 = 00;
    TIM3->CCER |= LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH4;
    TIM3->BDTR |= TIM_BDTR_MOE;
    TIM3->CR1 |= TIM_CR1_CEN;
    
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
        GPIOA->BSRR = (0xf << (9 + 16)) | (get_buttons() << 9);
    }
}
