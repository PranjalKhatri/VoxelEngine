#pragma once
#include <memory>
#include <unordered_map>
#include "biome.hpp"
#include "biome_resolver.hpp"
#include "biome_types.hpp"
namespace pop::biome {

class BiomeRegistry {
   public:
    static BiomeRegistry& Get() {
        static BiomeRegistry instance;
        return instance;
    }
    template <typename T>
    void Register(BiomeType type) {
        auto biomeInstance = std::make_unique<T>();
        auto props         = biomeInstance->GetProperties();

        BiomeResolver::Get().AddPoint(
            {(float)props.temperature, (float)props.precipitation, type});

        registry_[type] = std::move(biomeInstance);
    }

    const Biome& GetBiome(BiomeType type) const { return *registry_.at(type); }

   private:
    std::unordered_map<BiomeType, std::unique_ptr<Biome>> registry_;
};

}  // namespace pop::biome
