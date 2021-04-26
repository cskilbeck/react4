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

    virtual void start()
    {
    }

    virtual void tick()
    {
    }

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

    template <typename T> static void set()
    {
        state *s = new(state_buffer) T();
        waitvb = true;
        s->start();
    }

    static bool waitvb;
    static byte state_buffer[];
};

//////////////////////////////////////////////////////////////////////

#undef STATE
#define STATE(x)               \
    struct x : state           \
    {                          \
        void start() override; \
        void tick() override;  \
    };

#include "states.h"
#undef STATE

//////////////////////////////////////////////////////////////////////
