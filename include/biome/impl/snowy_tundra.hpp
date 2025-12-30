#pragma once

#include "../biome.hpp"

namespace pop::biome {
class TundraBiome : public pop::biome::Biome {
   public:
    TundraBiome()
        : Biome({.biomeName       = "Tundra",
                 .baseHeight      = 0.4f,
                 .heightVariation = 0.1f,
                 .temperature     = -15,
                 .precipitation   = 20,
                 .mapColor        = 0xBFE3E3}) {}

    block::BlockID GetBlockAt(float density, int depth,
                              int globalY) const override {
        if (depth == 0) return block::IDs::SNOW;
        if (depth < 3) return block::IDs::DIRT;  // Frozen soil
        return block::IDs::STONE;
    }
};
}  // namespace pop::biome
