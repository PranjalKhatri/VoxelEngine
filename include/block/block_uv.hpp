#pragma once

#include <cstdint>
namespace pop::block {
struct BlockUV {
    uint16_t top, bottom, north, south, east, west;
    // Constructor for "all sides same"
    constexpr BlockUV(uint16_t all)
        : top(all), bottom(all), north(all), south(all), east(all), west(all) {}

    // Constructor for column blocks (logs/pillars)
    constexpr BlockUV(uint16_t top_bottom, uint16_t sides)
        : top(top_bottom),
          bottom(top_bottom),
          north(sides),
          south(sides),
          east(sides),
          west(sides) {}

    // Fully unique (Furnace/Crafting Table)
    constexpr BlockUV(uint16_t t, uint16_t b, uint16_t n, uint16_t s,
                      uint16_t e, uint16_t w)
        : top(t), bottom(b), north(n), south(s), east(e), west(w) {}
};
}  // namespace pop::block
