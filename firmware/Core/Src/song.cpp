#include "main.h"
#include "util.h"
#include "song.h"

#define TIMER TIM14
#define PORT GPIOA
#define PIN 4

namespace
{
    int when;
    int notes_to_play;
    int tune_length;
    song::note const *next_note;
    song::note const *cur_note;
    song::note const *tune;
    song::loop loop_option = song::loop::single;

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

    void play(note const *song, size_t num, loop option)
    {
        loop_option = option;
        tune = song;
        tune_length = num;
        notes_to_play = num;
        next_note = song;
        cur_note = null;
        when = ticks;
        set_port_mode(LL_GPIO_MODE_ALTERNATE);
        TIM14->CR1 |= TIM_CR1_CEN;
    }
    
    void stop()
    {
        // switch GPIO into input mode because otherwise it bleeds
        // some stuff which you can hear
        set_port_mode(LL_GPIO_MODE_INPUT);

        // and switch off the timer
        TIMER->CR1 &= ~TIM_CR1_CEN;

        tune = null;
        tune_length = 0;
        notes_to_play = 0;
        loop_option = loop::single;
    }
    
    bool finished()
    {
        return notes_to_play == 0;
    }

    void update()
    {
        if(notes_to_play == 0) {
            if(loop_option == loop::single) {
                stop();
                return;
            }
            else if(loop_option == loop::looping && tune != null) {
                play(tune, tune_length, loop_option);
            }
        }

        uint32 t = (ticks * speed) >> 8;

        if(next_note != null) {
            TIM14->ARR = next_note->timer1;
            cur_note = next_note;
            next_note = null;
            when = t;
        }

        int played_time = t - when;

        if(played_time >= cur_note->delay) {
            notes_to_play -= 1;
            next_note = cur_note + 1;
        }

        // note slide
        int32 d = (played_time << 16) / cur_note->delay;    // 0..65536

        int x = (((cur_note->timer2 - cur_note->timer1) * d) >> 16) + cur_note->timer1;
        TIMER->ARR = x;
        
        // tone ramp
        int v = 32 - min(32, played_time * 4 / 32);
        v = 64 - ((((v * v) >> 5) * v) >> 5);

        TIMER->CCR1 = (cur_note->timer1 * v) / 128;
    }
}    // namespace song