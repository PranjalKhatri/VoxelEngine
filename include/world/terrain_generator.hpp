#pragma once
#include "fast_noise_lite.h"

namespace pop::voxel::terrain {
class TerrainGenerator {
   public:
    struct Properties {
        int   seed       = 1337;
        float frequency  = 0.02f;
        float heightBias = 64.0f;  // surface height
        float hardness   = 15.0f;  // how steep the density drop off is
    };
    TerrainGenerator(Properties properties);
    TerrainGenerator() : TerrainGenerator(Properties{}) {}

    TerrainGenerator(const TerrainGenerator&)            = delete;
    TerrainGenerator(TerrainGenerator&&)                 = delete;
    TerrainGenerator& operator=(const TerrainGenerator&) = delete;
    TerrainGenerator& operator=(TerrainGenerator&&)      = delete;

    float GetHeight(float x, float y, float z);

    float GetDensity(float x, float y, float z);

   private:
    void SetupNoise();

   private:
    FastNoiseLite noise_;
    Properties    properties_;
};
}  // namespace pop::voxel::terrain
