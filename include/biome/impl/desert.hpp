#pragma once

#include "../biome.hpp"

namespace pop::biome {
class DesertBiome : public pop::biome::Biome {
   public:
    DesertBiome()
        : Biome({.biomeName       = "Desert",
                 .baseHeight      = 0.2f,
                 .heightVariation = 0.05f,
                 .temperature     = 45,
                 .precipitation   = 10,
                 .mapColor        = 0xFAE251}) {}

    block::BlockID GetBlockAt(float density, int depth,
                              int globalY) const override {
        if (depth < 6) return block::IDs::SAND;
        return block::IDs::SANDSTONE;
    }
};
}  // namespace pop::biome
