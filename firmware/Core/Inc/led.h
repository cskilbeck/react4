#pragma once

#include "main.h"
#include "util.h"


// setup the timers, GPIO, IRQs
void led_init(void);

// 12 bits per led of brightness
extern uint16 led_brightness[16];
