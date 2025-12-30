#pragma once

#include "../biome.hpp"

namespace pop::biome {
class PlainsBiome : public pop::biome::Biome {
   public:
    PlainsBiome()
        : Biome({.biomeName       = "Plains",
                 .baseHeight      = 0.3f,
                 .heightVariation = 0.1f,
                 .temperature     = 20,
                 .precipitation   = 100,
                 .mapColor        = 0x91BD59}) {}

    // Uses default GetBlockAt (Grass -> Dirt -> Stone)
};
}  // namespace pop::biome
