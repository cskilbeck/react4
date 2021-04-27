#pragma once

#include "main.h"
#include "util.h"

namespace led
{
    void init();
    void cls(uint16 value = 0);
    void set(int col, int row, uint16 value);
    void set_snap(bool state);
    int gamma(int x);
    
}    // namespace led
