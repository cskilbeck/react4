//////////////////////////////////////////////////////////////////////
// game/score etc

#include <string.h>
#include "main.h"
#include "util.h"
#include "song.h"
#include "led.h"
#include "state.h"

//////////////////////////////////////////////////////////////////////

volatile unsigned long ticks = 0;

//////////////////////////////////////////////////////////////////////

extern "C" void user_main()
{
    // enable 1ms tick
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    NVIC_DisableIRQ(SysTick_IRQn);

    GPIOA->BSRR = 1 << 12;

    led::init();
    song::init();
    state::init();

    while(true) {
        song::update();
        state::update();
        __WFI();
    }
}
