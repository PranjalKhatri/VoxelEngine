#pragma once

#include "texture.hpp"
#include <cstdint>
#include <memory>
namespace pop::gfx::rtypes {

struct TextureBinding {
    std::shared_ptr<Texture> texture;
    uint8_t                  slot;  // GL_TEXTURE0 + slot
};
enum class MeshType : uint8_t { kSolidMesh = 0, kWaterMesh, kMeshCount };
constexpr static bool IsSolidMesh(const MeshType& tp) {
    return tp == MeshType::kSolidMesh;
}

}  // namespace pop::gfx::rtypes
