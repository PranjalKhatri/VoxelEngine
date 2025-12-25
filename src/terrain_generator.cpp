#include "voxel/terrain_generator.hpp"
#include "voxel/chunk.hpp"

namespace pop::voxel::terrain {

TerrainGenerator::TerrainGenerator() {
    noise_.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise_.SetFractalOctaves(5);
    noise_.SetFractalLacunarity(2.0f);
    noise_.SetFractalGain(0.5f);

    noise_.SetFrequency(kFrequency);
}
float TerrainGenerator::GetDensity(float x, float y, float z) {
    float n3d = noise_.GetNoise(x, y, z);

    float heightGradient = (y - kHeightBias) / kHardness;

    // Density = Noise - Gradient
    // Result: Positive at the bottom (solid), Negative at the top (air)
    return n3d - heightGradient;
}
float TerrainGenerator::GetHeight(float x, float y, float z) {
    return noise_.GetNoise(x, y, z);
}

};  // namespace pop::voxel::terrain
