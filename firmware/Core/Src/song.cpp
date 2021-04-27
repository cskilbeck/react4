#include "main.h"
#include "util.h"
#include "song.h"

#define TIMER TIM14
#define CHANNEL LL_TIM_CHANNEL_CH1
#define PORT GPIOA
#define PIN 4

namespace
{
    int constexpr speed = 18;

    song::note const *tune;
    int tune_length;
    song::option loop_option = song::option::single;

    int when;
    int notes_to_play;

    song::note const *next_note;
    song::note const *cur_note;

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
        TIMER->CCER |= CHANNEL;
        TIMER->BDTR |= TIM_BDTR_MOE;
    }

    void play(note const *song, size_t num, option opt)
    {
        loop_option = opt;
        tune = song;
        tune_length = num;
        notes_to_play = num;
        next_note = song;
        cur_note = null;
        when = (ticks * speed) >> 8;
        set_port_mode(LL_GPIO_MODE_ALTERNATE);
        TIMER->CR1 |= TIM_CR1_CEN;
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
        loop_option = option::single;
    }
    
    bool finished()
    {
        return notes_to_play == 0;
    }

    void update()
    {
        if(notes_to_play == 0) {
            if(loop_option == option::single) {
                stop();
                return;
            }
            else if(loop_option == option::looping && tune != null) {
                play(tune, tune_length, loop_option);
            }
        }

        uint32 t = (ticks * speed) >> 8;

        if(next_note != null) {
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