#pragma once

#include "main.h"
#include "util.h"

namespace song
{
    struct note
    {
        uint16 timer1;
        uint16 timer2;
        uint16 delay;
    };

    enum class option
    {
        single = 0,
        looping = 1
    };

    void play(note const *song, size_t num, option opt = option::single);
    template <size_t N> void play(note const (&song)[N], option opt = option::single)
    {
        play(song, N, opt);
    }

    bool finished();

    void stop();

    void init();
    void update();

};    // namespace song

namespace scale
{
    int constexpr note(float freq)
    {
        return int(48000000.0f / 44.0f / freq);
    }

    int constexpr LOW = note(150);
    int constexpr B3 = note(246.94);
    int constexpr C4 = note(261.63);
    int constexpr C_SHARP_4 = note(277.18);
    int constexpr D4 = note(293.66);
    int constexpr D_SHARP_4 = note(311.13);
    int constexpr E4 = note(329.63);
    int constexpr F4 = note(349.23);
    int constexpr F_SHARP_4 = note(369.99);
    int constexpr G4 = note(392);
    int constexpr G_SHARP_4 = note(415.3);
    int constexpr A4 = note(440);
    int constexpr A_SHARP_4 = note(466.16);
    int constexpr B4 = note(493.88);
    int constexpr C5 = note(523.25);
    int constexpr C_SHARP_5 = note(554.37);
    int constexpr D5 = note(587.33);
    int constexpr D_SHARP_5 = note(622.25);
    int constexpr E5 = note(659.25);
    int constexpr F5 = note(698.46);
    int constexpr F_SHARP_5 = note(739.99);
    int constexpr G5 = note(783.99);
    int constexpr G_SHARP_5 = note(830.61);
    int constexpr A5 = note(880);
    int constexpr A_SHARP_5 = note(932.33);
    int constexpr B5 = note(987.77);
    int constexpr C6 = note(1046.5);
    int constexpr C_SHARP_6 = note(1108.73);
    int constexpr D6 = note(1174.66);
    int constexpr D_SHARP_6 = note(1244.51);
    int constexpr E6 = note(1318.51);
    int constexpr F6 = note(1396.91);
    int constexpr F_SHARP_6 = note(1479.98);
    int constexpr G6 = note(1567.98);
    int constexpr G_SHARP_6 = note(1661.22);
    int constexpr A6 = note(1760);
    int constexpr A_SHARP_6 = note(1864.66);
    int constexpr B6 = note(1975.53);
    int constexpr C7 = note(2093);
    int constexpr C_SHARP_7 = note(2217.46);
    int constexpr D7 = note(2349.32);
    int constexpr D_SHARP_7 = note(2489.02);
    int constexpr E7 = note(2637.02);
    int constexpr F7 = note(2793.83);
    int constexpr F_SHARP_7 = note(2959.96);
    int constexpr G7 = note(3135.96);
    int constexpr G_SHARP_7 = note(3322.44);
    int constexpr A7 = note(3520);
    int constexpr A_SHARP_7 = note(3729.31);
    int constexpr B7 = note(3951.07);

}    // namespace scale
