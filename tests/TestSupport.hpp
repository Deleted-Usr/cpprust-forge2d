#pragma once

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

inline int checks = 0;

inline void check(bool condition, const std::string& message)
{
    ++checks;
    if (!condition) throw std::runtime_error(message);
}

inline bool near(float a, float b, float epsilon = 0.0001f)
{
    return std::abs(a - b) < epsilon;
}
