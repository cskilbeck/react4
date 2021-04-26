//////////////////////////////////////////////////////////////////////

#include "state.h"
#include "song.h"
#include "led.h"
#include "buttons.h"

//////////////////////////////////////////////////////////////////////

// clang-format off
song::note const charge[6] = {
    { 15306, 140 },
    { 11467, 140 },
    { 9101,  140 },
    { 7653,  280 },
    { 9101,  120 },
    { 7653,  500 }
};

song::note const bloop[1] = {
    { 15306, 140 }
};

// clang-format on

//////////////////////////////////////////////////////////////////////

struct test;
struct pulse;
struct waiting;
struct turn_begins;
struct snap;
struct winner;
struct loser;
struct game_over;

//////////////////////////////////////////////////////////////////////

struct test : state
{
    void start()
    {
        led::cls();
    }
    
    void tick()
    {
        int x = (ticks >> 2) & 4095;
        x = (x * x) >> 12;
        for(int i=0; i<4; ++i) {
            for(int j=0; j<4; ++j) {
                led_brightness[3][j] = x;
            }
        }
    }
};

//////////////////////////////////////////////////////////////////////

struct pulse : state
{
    void start()
    {
        song::play(charge);
    }

    void tick()
    {
        int z = min(4095U, state_ticks() >> 1);
        int q = (z * z) >> 12;
        for(int i=0; i<4; ++i) {
            for(int j=0; j<4; ++j) {
                led_brightness[i][j] = q;
            }
        }
        if(song::finished()) {
            state::set<waiting>();
        }
    }
};

//////////////////////////////////////////////////////////////////////

namespace game
{
    int score[4];

    void start()
    {
        memset(score, 0, sizeof(score));
        state::set<turn_begins>();
    }
}

//////////////////////////////////////////////////////////////////////

struct waiting : state
{
    void start()
    {
        memset(led_brightness, 0, sizeof(led_brightness));
    }

    void tick()
    {
        int t = state_ticks();
        int x = t >> 10;
        int col = x & 3;
        int row = (x >> 2) & 3;
        led_brightness[col][row] = 4000;
        if(x > 0 && (x & 15) == 0) {
            reset();
            led::cls();
        }
        if(get_buttons() != 0) {
            game::start();
        }
    }
};

//////////////////////////////////////////////////////////////////////

struct turn_begins : state
{
    void start()
    {
        led::cls();
    }
    
    void tick()
    {
        auto b = get_buttons();
        for(int i=0; i<4; ++i) {
            int m = 1 << i;
            if((b & m) != 0) {
                led_brightness[0][i] = 4000;
            }
        }
    }
};

//////////////////////////////////////////////////////////////////////

void state::init()
{
    state::set<pulse>();
}

//////////////////////////////////////////////////////////////////////

void state::update()
{
    reinterpret_cast<state *>(state_buffer)->tick();
}

//////////////////////////////////////////////////////////////////////

// clang-format off
size_t constexpr largest_state_size = sizeof(largest_type<
    pulse,
    waiting,
    turn_begins
//    snap,
//    winner,
//    loser,
//    game_over
>::type);
// clang-format on

byte state::state_buffer[largest_state_size];
