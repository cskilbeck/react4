#pragma once

#include "main.h"
#include "util.h"

namespace song
{
    struct note
    {
        uint16 timer;
        uint16 delay;
    };

    void play(note const *song, size_t num);
    template <size_t N> void play(note const (&song)[N])
    {
        play(song, N);
    }
    
    bool finished();

    void init();
    void update();

};    // namespace song
