//////////////////////////////////////////////////////////////////////

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
    // disable systick
    NVIC_DisableIRQ(SysTick_IRQn);
    SysTick->CTRL = 0;

    led::init();
    song::init();
    state::init();

    while(true) {
        song::update();
        state::update();
    }
}
