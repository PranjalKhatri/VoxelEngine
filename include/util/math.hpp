#pragma once

#include <concepts>
#include <cmath>

namespace pop::util {

/**
 * Ensures a positive modulo result even for negative inputs.
 */
template <typename T>
    requires std::integral<T>
constexpr T PositiveMod(T a, T b) {
    return (a % b + b) % b;
}

/**
 * Overload for floating-point types using std::fmod.
 */
template <typename T>
    requires std::floating_point<T>
T PositiveMod(T a, T b) {
    T res = std::fmod(a, b);
    return res < 0 ? res + b : res;
}

}  // namespace pop::util
