#pragma once
#include "block.hpp"
#include "block_ids.hpp"
#include <vector>
#include <memory>

namespace pop::block {

class BlockRegistry {
   public:
    static BlockRegistry& Get() {
        static BlockRegistry instance;
        return instance;
    }

    // Add a block type and return its new ID
    BlockID Register(std::unique_ptr<Block> block) {
        BlockID id = static_cast<BlockID>(blocks_.size());
        blocks_.push_back(std::move(block));
        return id;
    }

    const Block& GetBlock(BlockID id) const { return *blocks_.at(id); }

   private:
    BlockRegistry() = default;
    std::vector<std::unique_ptr<Block>> blocks_;
};
}  // namespace pop::block
