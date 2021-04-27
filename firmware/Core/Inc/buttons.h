#pragma once

#include "main.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////

static uint32 get_buttons()
{
    uint32 b = GPIOA->IDR & 0xf;
    b = (b >> 1) | ((b & 1) << 3);
    return b ^ 0xf;
}
