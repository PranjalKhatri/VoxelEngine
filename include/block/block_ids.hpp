#pragma once
#include <cstdint>

namespace pop::block {
using BlockID = uint16_t;

namespace IDs {
// These are assigned during the registration process in Engine probably(:[)
inline BlockID AIR   = 0;
inline BlockID GRASS = 0;
inline BlockID DIRT  = 0;
inline BlockID STONE = 0;
inline BlockID WATER = 0;
inline BlockID SAND  = 0;
}  // namespace IDs
}  // namespace pop::block
