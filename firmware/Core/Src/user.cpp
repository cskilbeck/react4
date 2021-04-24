//////////////////////////////////////////////////////////////////////


#include <string.h>
#include "main.h"
#include "util.h"
#include "song.h"

//////////////////////////////////////////////////////////////////////

volatile unsigned long ticks = 0;

//////////////////////////////////////////////////////////////////////

uint32 get_buttons()
{
    uint32 b = GPIOA->IDR & 0xf;
    b = (b >> 1) | ((b & 1) << 3);
    return b;
}

//////////////////////////////////////////////////////////////////////

// clang-format off
song::note const charge[6] = {
    { 15306, 140 },
    { 11467, 140 },
    { 9101,  140 },
    { 7653,  280 },
    { 9101,  120 },
    { 7653,  500 }
};
// clang-format on

//////////////////////////////////////////////////////////////////////

#define OP_ACTIVE_LOW 1
#define OP_ACTIVE_HIGH 0

//////////////////////////////////////////////////////////////////////

#define PWM_MODE 0b111
#define ON_SHIFT 16
#define OUTPUT_POLARITY OP_ACTIVE_HIGH

//////////////////////////////////////////////////////////////////////

#define PWM_MODE_SET0 ((PWM_MODE << TIM_CCMR1_OC1M_Pos) | (PWM_MODE << TIM_CCMR1_OC2M_Pos))
#define PWM_MODE_SET1 ((PWM_MODE << TIM_CCMR2_OC3M_Pos) | (PWM_MODE << TIM_CCMR2_OC4M_Pos))

#define OFF_SHIFT (16 - ON_SHIFT)

// clang-format off
#define OUTPUT_POLARITY_SET ( \
    (OUTPUT_POLARITY << TIM_CCER_CC1P_Pos) | \
    (OUTPUT_POLARITY << TIM_CCER_CC2P_Pos) | \
    (OUTPUT_POLARITY << TIM_CCER_CC3P_Pos) | \
    (OUTPUT_POLARITY << TIM_CCER_CC4P_Pos))
// clang-format on

#define CHANNEL_SET (TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E)

//////////////////////////////////////////////////////////////////////

struct io_setter
{
    GPIO_TypeDef *port;
    uint32 value;
    uint16 *leds;

    //////////////////////////////////////////////////////////////////////

    void clear()
    {
        port->BSRR = value << OFF_SHIFT;
    }

    //////////////////////////////////////////////////////////////////////

    void set()
    {
        uint16 *p = leds;
        TIM1->CCR1 = *p++;
        TIM1->CCR2 = *p++;
        TIM1->CCR3 = *p++;
        TIM1->CCR4 = *p++;
        port->BSRR = value << ON_SHIFT;
    }
};

//////////////////////////////////////////////////////////////////////

int current_column = 0;

uint16 led_brightness[4][4];

//////////////////////////////////////////////////////////////////////

// clang-format off
io_setter led_column[4] = {
    { GPIOB, 1 << 1, led_brightness[0] },
    { GPIOB, 1 << 0, led_brightness[1] },
    { GPIOA, 1 << 7, led_brightness[2] },
    { GPIOA, 1 << 6, led_brightness[3] }
};
// clang-format on

//////////////////////////////////////////////////////////////////////

extern "C" void TIM1_BRK_UP_TRG_COM_IRQHandler()
{
    //GPIOA->BSRR = 1 << (12+16);
    led_column[current_column].clear();    // disable previous column

    current_column = ++current_column & 3;

    led_column[current_column].set();    // setup next column

    TIM1->CR1 = TIM_CR1_CEN | TIM_CR1_OPM;    // kick timer1

    TIM1->SR = 0;    // clear timer1 irq
    
    ticks += 1;
}

//////////////////////////////////////////////////////////////////////

extern "C" void user_main()
{
    // enable 1ms tick
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    NVIC_DisableIRQ(SysTick_IRQn);

    for(int i = 0; i < 3; ++i) {
        led_column[i].clear();
    }
    
    GPIOA->BSRR = 1 << 12;

    // TIM1 for row sinks
    TIM1->CCMR1 = PWM_MODE_SET0;
    TIM1->CCMR2 = PWM_MODE_SET1;
    TIM1->CR1 = 0;
    TIM1->CNT = 0;
    TIM1->PSC = 0;
    TIM1->ARR = 4096;
    TIM1->CCER = CHANNEL_SET | OUTPUT_POLARITY_SET;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1 = TIM_CR1_CEN | TIM_CR1_OPM;

    // TIM1 Update IRQ enable
    TIM1->DIER = TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

    // TIM14 for buzzer
    TIM14->CCER |= LL_TIM_CHANNEL_CH1;
    TIM14->BDTR |= TIM_BDTR_MOE;
    TIM14->CR1 |= TIM_CR1_CEN;

    song s;
    s.init(charge);

    int row = 0;
    int col = 0;

    int now = ticks;
    
    int b = 0;

    while(true) {
        s.play();
        b += 1;
        int z = (b >> 1) & 4095;
        int q = (z * z) >> 12;
        for(int i=0; i<4; ++i) {
            for(int j=0; j<4; ++j) {
                led_brightness[i][j] = q;
            }
        }
//        if((ticks - now) > 1000) {
//            led_brightness[col][row] = 0;
//            if(++col == 4) {
//                col = 0;
//                row = ++row & 3;
//            }
//            led_brightness[col][row] = 4000;
//            now = ticks;
//        }
        
        __WFI();
    }
}
