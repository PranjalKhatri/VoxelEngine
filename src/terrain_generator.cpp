#include "terrain_generator.hpp"

namespace pop::voxel::terrain {

TerrainGenerator::TerrainGenerator() {
    noise_.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise_.SetFractalOctaves(5);
}
float TerrainGenerator::GetHeight(float x, float y) {
    return noise_.GetNoise(x, y);
}

};  // namespace pop::voxel::terrain
