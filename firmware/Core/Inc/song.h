#pragma once

#include "main.h"
#include "util.h"

struct song
{
    struct note
    {
        uint16 timer;
        uint16 delay;
    };

    int when = 0;
    int notes_to_play = 0;
    bool play_note = false;
    note const *cur_note = null;
    
    template<size_t N> void init(note const (&song)[N])
    {
        notes_to_play = N;
        cur_note = song;
        when = ticks;
        play_note = true;
    }
    
    void play()
    {
        if(notes_to_play == 0) {
            TIM14->CR1 &= ~TIM_CR1_CEN;
            return;
        }
        note const *n = cur_note;
        int t = ticks;

        if(play_note) {
            TIM14->ARR = n->timer;
            play_note = false;
        }

        int played_time = t - when;
 
        if(played_time >= n->delay) {
            play_note = true;
            cur_note += 1;
            notes_to_play -= 1;
            when = ticks;
        }
        
        // volume down ramp
        int v = 128 - min(128, played_time * 30 / 128);
        v = 256 - ((((v * v) >> 7) * v) >> 7);

        TIM14->CCR1 = (n->timer * v) / 256;
    }
};

