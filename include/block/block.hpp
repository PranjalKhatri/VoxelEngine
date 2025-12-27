#pragma once
#include <cstdint>
#include <string>
#include "block/block_uv.hpp"
#include "util/directions.hpp"

// TODO: Add a materaial class
namespace pop::block {
class Block {
   public:
    struct Settings {
        bool        isSolid       = true;
        bool        isTranslucent = false;
        BlockUV     uv            = 0;
        std::string name          = "Unknown";
    };

    Block(Settings settings) : settings_(settings) {}
    virtual ~Block() = default;

    bool    IsSolid() const { return settings_.isSolid; }
    bool    IsTranslucent() const { return settings_.isTranslucent; }
    uint8_t GetTextureCode(util::direction dir) const;

    const std::string& GetName() const { return settings_.name; }

   protected:
    Settings settings_;
};
}  // namespace pop::block
