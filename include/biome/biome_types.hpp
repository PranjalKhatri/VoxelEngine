#pragma once
#include <cstdint>
namespace pop::biome {
enum class BiomeType : uint8_t {
    kTundra = 0,
    kDesert,
    kRainforest,
    kPlains,
    kOcean,
    kCount
};
};
