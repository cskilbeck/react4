//////////////////////////////////////////////////////////////////////

#pragma once

#include "main.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////

struct state
{
    uint32 state_start_ticks;

    state()
    {
        reset();
    }

    virtual void start() = 0;
    virtual void tick() = 0;

    void reset()
    {
        state_start_ticks = ticks;
    }

    uint32 state_ticks()
    {
        return ticks - state_start_ticks;
    }

    static void init();
    static void update();

    template <typename T> static void set();

    static size_t constexpr largest_state_size = 32;

    static bool waitvb;
    static byte state_buffer[largest_state_size];
};

//////////////////////////////////////////////////////////////////////
// for debugging

struct test : state
{
    void start() override;
    void tick() override;
};

//////////////////////////////////////////////////////////////////////
// boot anim

struct startup : state
{
    void start() override;
    void tick() override;
};

//////////////////////////////////////////////////////////////////////
// attract mode

struct waiting : state
{
    uint button_idle;

    void start() override;
    void tick() override;
};

//////////////////////////////////////////////////////////////////////
// game starting

struct game_start : state
{
    void start() override;
    void tick() override;
};

//////////////////////////////////////////////////////////////////////
// turn starting (wait for buttons to all be released)

struct turn_begins : state
{
    uint button_idle;
    
    void start() override;
    void tick() override;
};

//////////////////////////////////////////////////////////////////////
// turn_wait

struct turn_wait : state
{
    void start() override;
    void tick() override;
};

//////////////////////////////////////////////////////////////////////
// light goes on

struct snap : state
{
    void start() override;
    void tick() override;
};

//////////////////////////////////////////////////////////////////////
// somebody won a point

struct win : state
{
    void start() override;
    void tick() override;
};

//////////////////////////////////////////////////////////////////////
// somebody jumped the gun

struct lose : state
{
    void start() override;
    void tick() override;
};

//////////////////////////////////////////////////////////////////////
// somebody won the game

struct game_over : state
{
    void start() override;
    void tick() override;
};

//////////////////////////////////////////////////////////////////////
// set a new state

template <typename T> void state::set()
{
    // largest_state_size big enough?
    static_assert(sizeof(T) <= largest_state_size, "Make largest_state_size bigger");

    // setup vtable
    state *s = new(state_buffer) T();

    // default waitvb true
    waitvb = true;

    s->start();
}
