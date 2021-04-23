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

struct io_setter
{
    GPIO_TypeDef *port;
    uint32 value;
    uint16 *leds;

    void clear()
    {
        port->BSRR = value;
    }

    void set()
    {
        uint16 *p = leds;
        TIM1->CCR1 = *p++;
        TIM1->CCR2 = *p++;
        TIM1->CCR3 = *p++;
        TIM1->CCR4 = *p++;
        port->BSRR = value << 16;
    }
};

int current_column = 0;

uint16 led_brightness[4][4];

io_setter led_column[4] = 
{
    {GPIOA, 1<<6, led_brightness[1] },
    {GPIOA, 1<<7, led_brightness[2] },
    {GPIOB, 1<<0, led_brightness[3] },
    {GPIOB, 1<<1, led_brightness[0] }
};

extern "C" void TIM1_BRK_UP_TRG_COM_IRQHandler()
{
    led_column[current_column].clear();        // disable previous column

    current_column = ++current_column & 3;

    led_column[current_column].set();          // setup next column

    TIM1->SR = 0;               // clear timer1 irq
}

extern "C" void user_main()
{
    // enable 1ms tick
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    NVIC_EnableIRQ(SysTick_IRQn);

    // GPIOB[0..1], GPIOA[6..7] for column drivers
    GPIOB->BSRR = 3;
    GPIOA->BSRR = 3<<6;

    // TIM1 for row sinks
    TIM1->CR1 = 0;
    TIM1->CNT = 0;
    TIM1->PSC = 0;
    TIM1->ARR = 4000;
    TIM1->CCER |= LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH4;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1 = TIM_CR1_CEN;

    // TIM1 Update IRQ enable
    TIM1->DIER = TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

    // TIM14 for buzzer
    TIM14->CCER |= LL_TIM_CHANNEL_CH1;
    TIM14->BDTR |= TIM_BDTR_MOE;
    TIM14->CR1 |= TIM_CR1_CEN;
    
    song s;
    s.init(charge);

    for(int i=0; i<4; ++i) {
        for(int j=0; j<4; ++j) {
            led_brightness[i][j] = 0;
        }
    }
    
    led_brightness[2][3] = 2000;

    while(true) {
        s.play();
        //GPIOA->BSRR = (0xf << (9 + 16)) | (get_buttons() << 9);
        __WFI();
    }
}
