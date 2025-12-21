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

    float GetHeight(float x, float y);

   private:
    TerrainGenerator();

    FastNoiseLite noise_;
};
}  // namespace pop::voxel::terrain
