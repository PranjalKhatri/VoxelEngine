#pragma once

#include "graphics/camera.hpp"
#include "core/engine.hpp"
#include "voxel/chunk.hpp"
#include "gl/gl_types.hpp"
#include "glm/fwd.hpp"
#include "graphics/rendertypes.hpp"

#include <cstddef>
#include <memory>
#include <unordered_map>

namespace pop::voxel {
struct ChunkCoord {
    int  x, z;
    bool operator==(const ChunkCoord& other) const noexcept {
        return x == other.x && z == other.z;
    }
};

inline ChunkCoord WorldToChunkCoord(const glm::ivec3& worldPoint) {
    return {static_cast<int>(
                std::floor(static_cast<float>(worldPoint.x) / Chunk::kSize_x)),
            static_cast<int>(
                std::floor(static_cast<float>(worldPoint.z) / Chunk::kSize_z))};
}
inline glm::ivec3 ChunkToOffset(const ChunkCoord& coord) {
    return {coord.x * Chunk::kSize_x, 0, coord.z * Chunk::kSize_z};
}
struct ChunkCoordHash {
    size_t operator()(const ChunkCoord& c) const noexcept {
        size_t h1 = std::hash<int>{}(c.x);
        size_t h2 = std::hash<int>{}(c.z);
        return h1 ^ (h2 << 1);
    }
};
class ChunkManager {
   public:
    ChunkManager(const gfx::FlyCam* player_cam_);
    ~ChunkManager() = default;

    void Run(core::Engine& engine);
    void SetShader(gfx::rtypes::MeshType meshType, gfx::ShaderHandle handle);
    void SetTexture(std::shared_ptr<gfx::rtypes::TextureBinding> texture);

    static constexpr int RenderDistance = 8;

   private:
    void LoadChunk(const ChunkCoord& coord, core::Engine& engine);
    void UnLoadChunk(const ChunkCoord& chunkCoord, core::Engine& engine);

    std::unique_ptr<Chunk> GenerateChunk(const ChunkCoord& chunkCoord);

    bool IsChunkLoaded(const ChunkCoord& chunkCoord);

   private:
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash>
                                                 loaded_chunks_;
    std::shared_ptr<gfx::rtypes::TextureBinding> tex_;
    const gfx::FlyCam*                           player_cam_;
    std::array<gfx::ShaderHandle,
               static_cast<size_t>(gfx::rtypes::MeshType::kMeshCount)>
        shader_handles_{};
};
};  // namespace pop::voxel
