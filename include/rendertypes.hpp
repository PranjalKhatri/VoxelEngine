#pragma once

#include "texture.hpp"
#include <cstdint>
#include <memory>
namespace pop::gfx::rtypes {

struct TextureBinding {
    std::shared_ptr<Texture> texture;
    uint8_t                  slot;  // GL_TEXTURE0 + slot
};

}  // namespace pop::gfx::rtypes
