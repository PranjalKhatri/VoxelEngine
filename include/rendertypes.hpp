#pragma once

#include "texture.hpp"
#include <cstdint>
#include <memory>
namespace pop::gfx::rtypes {

struct TextureBinding {
    std::shared_ptr<Texture> texture;
    uint8_t                  slot;  // GL_TEXTURE0 + slot
};

enum class VoxelMaterial : uint8_t { kNothing, kLand, kWater, kNumShaderTypes };

}  // namespace pop::gfx::rtypes
