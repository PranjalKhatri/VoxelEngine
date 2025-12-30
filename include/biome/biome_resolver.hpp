#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include "biome_types.hpp"

namespace pop::biome {

struct BiomePoint {
    float     tempC;
    float     precipCm;
    BiomeType type;
    int       biomeColor = 0x800080;
};

class BiomeResolver {
   public:
    static BiomeResolver& Get() {
        static BiomeResolver instance;
        return instance;
    }
    void      AddPoint(const BiomePoint& point);
    // generate the voronoi map and lookup table
    void      Bake(std::string_view outputPath = "BiomeDiagram.ppm");
    BiomeType GetBiomeAt(float tempC, float precipCm) const;

   private:
    BiomeResolver() = default;
    void WriteToFile(std::string_view filePath);
    int  Index(int x, int y) const { return x * kTableSize + y; }

    std::unique_ptr<uint8_t[]> lookup_table_;
    std::vector<BiomePoint>    points_;

    const int kTableSize = 400;

    const float kMinTemp   = -30.0f;
    const float kMaxTemp   = 50.0f;
    const float kTempRange = 80.0f;

    const float kMaxPrecip = 400.0f;
};

}  // namespace pop::biome
