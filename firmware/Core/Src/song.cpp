#include "main.h"
#include "util.h"
#include "song.h"

#define TIMER TIM14
#define PORT GPIOA
#define PIN 4

namespace
{
    int when = 0;
    int notes_to_play = 0;
    song::note const *next_note = null;
    song::note const *cur_note = null;

    constexpr int speed = 18;

    void set_port_mode(uint32 mode)
    {
        PORT->MODER = PORT->MODER & ~(GPIO_MODER_MODER0_Msk << (PIN * 2)) | (mode << (PIN * 2));
    }

}    // namespace

namespace song
{
    void init()
    {
        // TIM14 for buzzer
        TIM14->CCER |= LL_TIM_CHANNEL_CH1;
        TIM14->BDTR |= TIM_BDTR_MOE;
        TIM14->CR1 |= TIM_CR1_CEN;
    }

    void play(note const *song, size_t num)
    {
        notes_to_play = num;
        next_note = song;
        cur_note = null;
        when = ticks;
        set_port_mode(LL_GPIO_MODE_ALTERNATE);
        TIM14->CR1 |= TIM_CR1_CEN;
    }
    
    bool finished()
    {
        return notes_to_play == 0;
    }

    void update()
    {
        if(notes_to_play == 0) {
            // switch GPIO into input mode because otherwise it bleeds
            // some stuff which you can hear
            set_port_mode(LL_GPIO_MODE_INPUT);

            // and switch off the timer
            TIMER->CR1 &= ~TIM_CR1_CEN;
            return;
        }

        uint32 t = (ticks * speed) >> 8;

        if(next_note != null) {
            TIM14->ARR = next_note->timer;
            cur_note = next_note;
            next_note = null;
            when = t;
        }

        int played_time = t - when;

        if(played_time >= cur_note->delay) {
            notes_to_play -= 1;
            next_note = cur_note + 1;
        }

        // volume down ramp
        int v = 128 - min(128, played_time * 13 / 128);
        v = 256 - ((((v * v) >> 7) * v) >> 7);

        TIMER->CCR1 = (cur_note->timer * v) / 256;
    }
}    // namespace song