#pragma once

#include <string>
#include "block/block_ids.hpp"

namespace pop::biome {
class Biome {
   public:
    struct Properties {
        std::string biomeName       = "Base Biome";
        float       baseHeight      = 0.5f;
        float       heightVariation = 0.1f;
        int         temperature     = 30.0f;
        int         precipitation   = 200.0f;
    };
    Biome(Properties properties) : properties_(properties) {}
    virtual ~Biome();
    // Get the name of this biome
    std::string GetName() const { return properties_.biomeName; }
    Properties  GetProperties() const { return properties_; }

    /**
     * @param density The raw 3D noise value (usually > 0 is solid)
     * @param depth   Vertical distance from the highest surface at (x,z)
     * @param globalY The current Y world-coordinate
     */
    virtual block::BlockID GetBlockAt(float density, int depth,
                                      int globalY) const;

   private:
    Properties properties_;
};
}  // namespace pop::biome
