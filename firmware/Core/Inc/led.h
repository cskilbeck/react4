#pragma once

#include "main.h"
#include "util.h"

namespace led
{
    void init();
    void cls(uint16 value = 0);

    void set_snap(bool state);
}    // namespace led

// 12 bits per led of brightness
extern uint16 led_brightness[4][4];
