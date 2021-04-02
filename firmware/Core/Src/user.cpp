#include "main.h"
#include "util.h"
#include "song.h"

volatile unsigned long ticks = 0;

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

song::note const charge[6] = {
    { 15306	,140} ,
    { 11467	,140} ,
    { 9101	,140} ,
    { 7653	,280} ,
    { 9101	,120} ,
    { 7653	,500}
};

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
    
    song s;
    s.init(charge);

    while(true) {
        s.play();
        GPIOA->BSRR = (0xf << (9 + 16)) | (get_buttons() << 9);
        __WFI();
    }
}
