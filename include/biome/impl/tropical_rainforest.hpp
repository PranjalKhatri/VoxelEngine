#pragma once

#include "../biome.hpp"

namespace pop::biome {
class RainforestBiome : public pop::biome::Biome {
   public:
    RainforestBiome()
        : Biome({.biomeName       = "Rainforest",
                 .baseHeight      = 0.5f,
                 .heightVariation = 0.4f,
                 .temperature     = 30,
                 .precipitation   = 350,
                 .mapColor        = 0x07592B}) {}

    block::BlockID GetBlockAt(float density, int depth,
                              int globalY) const override {
        if (depth == 0) return block::IDs::GRASS;
        if (depth < 5) return block::IDs::DIRT;  // Rich, deep soil
        return block::IDs::STONE;
    }
};
}  // namespace pop::biome
