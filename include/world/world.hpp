#pragma once
#include "climate.hpp"
#include "biome/biome_reigstry.hpp"
#include "biome/biome_resolver.hpp"
#include "terrain_generator.hpp"

namespace pop::world {

class World {
   public:
    static World& Get() {
        static World instance;
        return instance;
    }

    // Initializes all sub-systems
    void Initialize() {
        pop::biome::RegisterAllBiomes();
        pop::biome::BiomeResolver::Get().Bake();
    }

    climate::ClimateModule&           GetClimate() { return climate_; }
    voxel::terrain::TerrainGenerator& GetGenerator() { return generator_; }

    pop::biome::BiomeRegistry& GetBiomeRegistry() {
        return pop::biome::BiomeRegistry::Get();
    }
    pop::biome::BiomeResolver& GetBiomeResolver() {
        return pop::biome::BiomeResolver::Get();
    }

   private:
    World() : climate_(), generator_() {}

    climate::ClimateModule           climate_;
    voxel::terrain::TerrainGenerator generator_;
};

}  // namespace pop::world
