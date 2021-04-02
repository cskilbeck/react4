#pragma once

using uint = unsigned int;
using int64 = int64_t;
using int32 = int32_t;
using int16 = int16_t;
using int8 = int8_t;
using uint64 = uint64_t;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8 = uint8_t;
using byte = uint8_t;
using size_t = uint32;
    
constexpr auto null = nullptr;

extern volatile unsigned long ticks;

template <typename T, size_t N> constexpr size_t countof(T const (&)[N])
{
    return N;
}

template <typename T> T min(T a, T b)
{
    return a < b ? a : b;
}

template <typename T> T max(T a, T b)
{
    return a > b ? a : b;
}
