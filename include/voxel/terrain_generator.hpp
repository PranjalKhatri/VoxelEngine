#pragma once
#include "fast_noise_lite.h"

namespace pop::voxel::terrain {
class TerrainGenerator {
   public:
    static TerrainGenerator& GetInstance() {
        static TerrainGenerator instance;
        return instance;
    }

    TerrainGenerator(const TerrainGenerator&)            = delete;
    TerrainGenerator(TerrainGenerator&&)                 = delete;
    TerrainGenerator& operator=(const TerrainGenerator&) = delete;
    TerrainGenerator& operator=(TerrainGenerator&&)      = delete;

    float GetHeight(float x, float y, float z);

    float GetDensity(float x, float y, float z);

   private:
    TerrainGenerator();
    FastNoiseLite noise_;
    const float   kFrequency  = 0.02f;
    const float   kHeightBias = 64.0f;  // Surface targets roughly y=64
    const float   kHardness   = 15.0f;  // How "steep" the density drop-off is
};
}  // namespace pop::voxel::terrain
