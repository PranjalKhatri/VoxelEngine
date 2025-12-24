#include "voxel/terrain_generator.hpp"
#include <cstdlib>
#include <ctime>

namespace pop::voxel::terrain {

TerrainGenerator::TerrainGenerator() {
    srand(time(0));
    noise_.SetSeed(rand());
    noise_.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise_.SetFractalOctaves(5);
}
float TerrainGenerator::GetHeight(float x, float y) {
    return noise_.GetNoise(x, y);
}

};  // namespace pop::voxel::terrain
