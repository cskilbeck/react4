//////////////////////////////////////////////////////////////////////
// ticks increments at 0.0853333 ms (11718.75Hz)
// so 1 second is ~11719 ticks

#include "state.h"
#include "song.h"
#include "led.h"
#include "buttons.h"

//////////////////////////////////////////////////////////////////////

using BOOT_STATE = startup;

//////////////////////////////////////////////////////////////////////

namespace
{
    uint constexpr one_second = 11719;
    uint constexpr half_second = one_second / 2;
    uint constexpr three_seconds = one_second * 4;
}    // namespace

//////////////////////////////////////////////////////////////////////

namespace game
{
    byte score[4];
    byte buttons;
    int delay;

    void new_game()
    {
        memset(score, 0, sizeof(score));
        random::seed(ticks);
        state::set<game_start>();
    }

    void start_turn()
    {
        delay = (random::get() % three_seconds) + one_second + half_second;    // 1.5 .. 4.5 seconds
    }
}    // namespace game

//////////////////////////////////////////////////////////////////////

#include "songs.inl"

//////////////////////////////////////////////////////////////////////
// do something for each bit set in game::buttons

template <typename T> static void for_each_bit(T const &fn)
{
    for(int i = 0; i < 4; ++i) {
        if((game::buttons & (1 << i)) != 0) {
            fn(i);
        }
    }
}

//////////////////////////////////////////////////////////////////////
// test
//////////////////////////////////////////////////////////////////////

void test::start()
{
    led::cls();
}

void test::tick()
{
}

//////////////////////////////////////////////////////////////////////
// startup
//////////////////////////////////////////////////////////////////////

void startup::start()
{
    song::play(tune::charge);
}

void startup::tick()
{
    int z = min(4095U, state_ticks() >> 1);
    int q = (z * z) >> 12;
    led::cls(q);
    if(song::finished()) {
        state::set<waiting>();
    }
}

//////////////////////////////////////////////////////////////////////
// waiting
//////////////////////////////////////////////////////////////////////

void waiting::start()
{
    led::cls();
}

void waiting::tick()
{
    int t = state_ticks();
    uint x = t >> 10;
    int col = x & 3;
    int row = (x >> 2) & 3;
    led_brightness[col][row] = 4000;
    led::set_snap(col == 0 && row == 0);
    if(x > 0 && (x & 15) == 0) {
        reset();
        led::cls();
    }
    if(get_buttons() != 0) {
        game::new_game();
    }
}

//////////////////////////////////////////////////////////////////////
// game_start
//////////////////////////////////////////////////////////////////////

void game_start::start()
{
    led::set_snap(false);
    song::play(tune::new_game);
}

void game_start::tick()
{
    int s = (state_ticks() >> 1) & 4095;
    int level = 4095 - s;
    level = (level * level) >> 12;
    led::cls(level);
    if(get_buttons() != 0) {
        reset();
    }
    if(level == 0 && song::finished()) {
        led::cls();
        state::set<turn_begins>();
    }
}

//////////////////////////////////////////////////////////////////////
// turn_begins
//////////////////////////////////////////////////////////////////////

void turn_begins::start()
{
    song::play(tune::waiting_to_start, song::option::looping);
    game::start_turn();
    waitvb = false;
}

void turn_begins::tick()
{
    uint32 b = get_buttons();
    if(b != 0) {
        game::buttons = b;
        state::set<lose>();
    }
    if(state_ticks() > game::delay) {
        state::set<snap>();
    }
}

//////////////////////////////////////////////////////////////////////
// snap
//////////////////////////////////////////////////////////////////////

void snap::start()
{
    song::play(tune::snap, song::option::looping);
}

void snap::tick()
{
    led::set_snap(((state_ticks() >> 8) & 3) == 0);
    uint32 b = get_buttons();
    if(b != 0) {
        game::buttons = b;
        state::set<win>();
    }
    if(state_ticks() > (one_second * 15)) {
        song::stop();
        led::set_snap(false);
        led::cls();
        state::set<waiting>();
    }
}

//////////////////////////////////////////////////////////////////////
// win
// add one to the winners, everyone else [loses one | stays the same?]
//////////////////////////////////////////////////////////////////////

void win::start()
{
    song::play(tune::winner);
    led::set_snap(true);
}

void win::tick()
{
    int flash = (((state_ticks() >> 9) & 1) != 0) ? 0 : 4095;

    for_each_bit([=](int i) { led_brightness[game::score[i]][i] = flash; });
    if(song::finished()) {
        bool game_won = false;
        for_each_bit([&](int i) {
            led_brightness[game::score[i]][i] = 4095;
            if(++game::score[i] == 4) {
                game_won = true;
            }
        });
        led::set_snap(false);
        if(game_won) {
            state::set<game_over>();
        } else {
            state::set<turn_begins>();
        }
    }
}

//////////////////////////////////////////////////////////////////////
// lose
// take one away from the losers, everyone else stays the same
//////////////////////////////////////////////////////////////////////

void lose::start()
{
    song::play(tune::loser);
    led::set_snap(false);
}

void lose::tick()
{
    int level = 4095 - min(4095, int(state_ticks() >> 2));
    level = (level * level) >> 12;
    for_each_bit([&](int i) {
        if(game::score[i] > 0) {
            led_brightness[game::score[i] - 1][i] = level;
        }
    });
    if(level == 0 && song::finished()) {
        for_each_bit([&](int i) { game::score[i] = max(0, game::score[i] - 1); });
        state::set<turn_begins>();
    }
}

//////////////////////////////////////////////////////////////////////
// game_over
//////////////////////////////////////////////////////////////////////

void game_over::start()
{
    song::play(tune::game_over);
}

void game_over::tick()
{
    int level = state_ticks() & 8191;
    level = abs(level - 4096);
    level = (level * level) >> 12;
    for(int i = 0; i < 4; ++i) {
        if(game::score[i] == 4) {
            for(int j = 0; j < 4; ++j) {
                led_brightness[j][i] = level;
            }
        }
    }
    if(state_ticks() > (one_second * 2) && get_buttons() != 0) {
        state::set<waiting>();
    }
}

//////////////////////////////////////////////////////////////////////

void state::init()
{
    state::set<BOOT_STATE>();
}

//////////////////////////////////////////////////////////////////////

void state::update()
{
    reinterpret_cast<state *>(state_buffer)->tick();
    if(waitvb) {
        __WFI();
    }
}

//////////////////////////////////////////////////////////////////////
// buffer for the state has to contain the largest of the state structs

#undef STATE
#define STATE(x) , x
size_t constexpr largest_state_size = sizeof(largest_type<byte
#include "states.h"
                                                          >::type);

byte state::state_buffer[largest_state_size];
#undef STATE

bool state::waitvb;
