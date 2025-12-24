#pragma once

#include <cstdint>
namespace pop {
enum class direction : uint8_t {
    kTop    = 0,
    kBottom = 1,
    kNorth  = 2,
    kSouth  = 3,
    kWest   = 4,
    kEast   = 5,

    kCount
};
}
