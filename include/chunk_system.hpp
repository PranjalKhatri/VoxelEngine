#pragma once

#include "camera.hpp"
#include "engine.hpp"
#include "chunk.hpp"
#include "gl_types.hpp"
#include "glm/fwd.hpp"
#include "rendertypes.hpp"

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

    void Run(Engine& engine);
    void SetShader(gfx::ShaderHandle handle);
    void SetTexture(std::shared_ptr<gfx::rtypes::TextureBinding> texture);

    static constexpr int RenderDistance = 16;

   private:
    void LoadChunk(const ChunkCoord& coord, Engine& engine);
    void UnLoadChunk(const ChunkCoord& chunkCoord, Engine& engine);

    std::unique_ptr<Chunk> GenerateChunk(const ChunkCoord& chunkCoord);

    bool IsChunkLoaded(const ChunkCoord& chunkCoord);

   private:
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash>
                                                 loaded_chunks_;
    std::shared_ptr<gfx::rtypes::TextureBinding> tex_;
    const gfx::FlyCam*                           player_cam_;
    gfx::ShaderHandle                            shader_handle_;
};
};  // namespace pop::voxel
