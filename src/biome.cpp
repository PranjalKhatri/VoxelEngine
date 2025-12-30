#include "biome/biome.hpp"
#include "biome/biome_reigstry.hpp"
#include "biome/biome_types.hpp"
#include "biome/impl/desert.hpp"
#include "biome/impl/plains.hpp"
#include "biome/impl/snowy_tundra.hpp"
#include "biome/impl/tropical_rainforest.hpp"
#include "block/block_ids.hpp"

namespace pop::biome {
block::BlockID Biome::GetBlockAt(float density, int depth,
                                 int /*globalY*/) const {
    if (density <= 0.0f) return block::IDs::AIR;

    if (depth == 0) return block::IDs::GRASS;
    if (depth < 4) return block::IDs::DIRT;
    return block::IDs::STONE;
}

void RegisterAllBiomes() {
    auto &reg = BiomeRegistry::Get();

    reg.Register<PlainsBiome>(BiomeType::kPlains);
    reg.Register<DesertBiome>(BiomeType::kDesert);
    reg.Register<RainforestBiome>(BiomeType::kRainforest);
    reg.Register<TundraBiome>(BiomeType::kTundra);
}
};  // namespace pop::biome
