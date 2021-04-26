//////////////////////////////////////////////////////////////////////
// 16 leds
// 4 per column (anode)
// 4 per row (cathode)
// TIM1 for rows
// TIM3 for columns
// TIM1 0..4095, upcounting
// TIM3 0..2048, center mode
// TIM3 reset slaved to TIM1
// TIM1 update event dma enables TIM3 channels each in turn
// TIM3 CCR values enable a short delay between columns to allow for slow fall time on mosfets

#include <string.h>
#include "main.h"
#include "util.h"
#include "led.h"

//////////////////////////////////////////////////////////////////////

uint16 led_brightness[4][4];

//////////////////////////////////////////////////////////////////////

namespace
{
    uint16 dma_x[4] = { TIM_CCER_CC4E | TIM_CCER_CC4P, TIM_CCER_CC3E | TIM_CCER_CC3P, TIM_CCER_CC2E | TIM_CCER_CC2P, TIM_CCER_CC1E | TIM_CCER_CC1P };

    //////////////////////////////////////////////////////////////////////

    static int current_column = 0;

    //////////////////////////////////////////////////////////////////////

    // clang-format off
    uint16 *led_column[4] = {
        led_brightness[3],
        led_brightness[0],
        led_brightness[1],
        led_brightness[2]
    };
    // clang-format on

}    // namespace

//////////////////////////////////////////////////////////////////////

namespace led
{
    void init()
    {
        DMA1_Channel5->CNDTR = 4;
        DMA1_Channel5->CMAR = (uint32)dma_x;
        DMA1_Channel5->CPAR = (uint32) & (TIM3->CCER);
        DMA1_Channel5->CCR = DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_CIRC | DMA_CCR_MINC | DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0 | DMA_CCR_PL_1;

        // TIM1 for row sinks
        TIM1->CR1 = 0;
        TIM1->BDTR |= TIM_BDTR_MOE;
        TIM1->DIER = TIM_DIER_UIE | TIM_DIER_UDE;
        TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E | TIM_CCER_CC1P | TIM_CCER_CC2P | TIM_CCER_CC3P | TIM_CCER_CC4P;
        TIM1->CR1 = TIM_CR1_CEN;

        NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

        TIM3->CR1 = 0;
        TIM3->BDTR |= TIM_BDTR_MOE;
        TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_CMS_0 | TIM_CR1_CMS_1;
    }

    void cls(uint16 value)
    {
        for(int i=0; i<4; ++i) {
            for(int j=0; j<4; ++j) {
                led_brightness[i][j] = value;
            }
        }
    }
    
    void set_snap(bool state)
    {
        GPIOA->BSRR = 1 << (12 + (state ? 16 : 0));
    }

}    // namespace led

//////////////////////////////////////////////////////////////////////

extern "C" void TIM1_BRK_UP_TRG_COM_IRQHandler()
{
    current_column = ++current_column & 3;

    uint16 *p = led_column[current_column];

    TIM1->CCR1 = *p++;
    TIM1->CCR2 = *p++;
    TIM1->CCR3 = *p++;
    TIM1->CCR4 = *p++;

    ticks += 1;

    TIM1->SR = 0;    // clear timer1 irq
}
