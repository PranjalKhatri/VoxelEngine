#include "block/block_loader.hpp"
#include "block/block_registry.hpp"
#include "block/block_ids.hpp"

namespace pop::block {

void RegisterAllBlocks() {
    auto& reg = BlockRegistry::Get();

    IDs::AIR   = reg.Register(std::make_unique<Block>(Block::Settings{
          .isSolid = false, .isTranslucent = true, .name = "Air"}));
    IDs::GRASS = reg.Register(
        std::make_unique<Block>(Block::Settings{.isSolid       = true,
                                                .isTranslucent = false,
                                                .uv            = {2, 3},
                                                .name          = "Grass"}));
    IDs::DIRT  = reg.Register(std::make_unique<Block>(Block::Settings{
         .isSolid = true, .isTranslucent = false, .uv = {4}, .name = "Dirt"}));
    IDs::WATER = reg.Register(std::make_unique<Block>(Block::Settings{
        .isSolid = false, .isTranslucent = true, .name = "Water"}));
    IDs::SAND  = reg.Register(std::make_unique<Block>(Block::Settings{
         .isSolid = true, .isTranslucent = false, .uv = {1}, .name = "Sand"}));
    IDs::STONE = reg.Register(std::make_unique<Block>(Block::Settings{
        .isSolid = true, .isTranslucent = false, .uv = {0}, .name = "Stone"}));
    // Add more as needed...
}
}  // namespace pop::block
