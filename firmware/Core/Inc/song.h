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
    note const *next_note = null;
    note const *cur_note = null;
    
    static constexpr int speed = 18;
    
    void init(note const *song, size_t num)
    {
        notes_to_play = num;
        next_note = song;
        cur_note = null;
        when = ticks;
    }

    template<size_t N> void init(note const (&song)[N])
    {
        init(song, N);
    }
    
    void play()
    {
        if(notes_to_play == 0) {
            TIM14->CR1 &= ~TIM_CR1_CEN;
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

        TIM14->CCR1 = (cur_note->timer * v) / 256;
    }
};

