#pragma once

#include "glm/common.hpp"
#include "graphics/camera.hpp"
#include "core/engine.hpp"
#include "util/math.hpp"
#include "util/safe_queue.hpp"
#include "voxel/chunk.hpp"
#include "gl/gl_types.hpp"
#include "glm/fwd.hpp"
#include "graphics/rendertypes.hpp"

#include <cstddef>
#include <memory>
#include <unordered_map>

namespace pop::voxel {
struct ChunkCoord {
    int        x, z;
    ChunkCoord operator+(const ChunkCoord& other) const {
        return ChunkCoord{x + other.x, z + other.z};
    }
    ChunkCoord operator-(const ChunkCoord& other) const {
        return ChunkCoord{x - other.x, z - other.z};
    }
    bool operator==(const ChunkCoord& other) const noexcept {
        return x == other.x && z == other.z;
    }
};

inline constexpr ChunkCoord WorldToChunkCoord(const glm::ivec3& worldPoint) {
    return {static_cast<int>(
                std::floor(static_cast<float>(worldPoint.x) / Chunk::kSize_x)),
            static_cast<int>(
                std::floor(static_cast<float>(worldPoint.z) / Chunk::kSize_z))};
}
inline constexpr glm::ivec3 WorldToChunkLocal(const glm::ivec3& worldPoint) {
    return {util::PositiveMod(worldPoint.x, Chunk::kSize_x),
            util::PositiveMod(worldPoint.y, Chunk::kSize_y),
            util::PositiveMod(worldPoint.z, Chunk::kSize_z)};
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
    struct ChunkBlockCmd {
        glm::vec3   position;
        glm::vec3   direction;
        Voxel::Type voxelToSet;
    };
    static constexpr int RenderDistance = 8;

    ChunkManager(const gfx::FlyCam* playerCam);
    ~ChunkManager() = default;

    void Run(core::Engine& engine);
    void SetShader(gfx::rtypes::MeshType meshType, gfx::ShaderHandle handle);
    void SetTexture(std::shared_ptr<gfx::rtypes::TextureBinding> texture);
    void AddChunkBlockCmd(const ChunkBlockCmd& cmd);

   private:
    void BreakBlock(glm::vec3 position, glm::vec3 dir);
    void ProcessCommands();
    void ProcessDirtyChunks(core::Engine& engine);
    void ProcessNewChunks(core::Engine& engine);
    void UploadChunkToEngine(const ChunkCoord& coord, core::Engine& engine,
                             bool Update = false);
    // WARN:Doesn't erase from the loaded_chunks
    void UnLoadChunk(const ChunkCoord& chunkCoord, core::Engine& engine);
    // Helper to get raw ptr from the map
    Chunk* GetRawChunkPtr(const ChunkCoord& coord);

    std::unique_ptr<Chunk> GenerateChunk(const ChunkCoord& chunkCoord);

    void LinkChunkNeighbors(const ChunkCoord& coord);
    void LinkAndMesh(const ChunkCoord& coord, core::Engine& engine);
    void MarkDirty(const ChunkCoord& coord, bool markAll = true,
                   const glm::ivec3& blockUpdated = {0, 0, 0});

    void InitialLoad(core::Engine& engine);
    bool IsChunkLoaded(const ChunkCoord& chunkCoord);

   private:
    util::CmdQueue<ChunkBlockCmd>                  chunkCmdQ{};
    std::unordered_set<ChunkCoord, ChunkCoordHash> dirty_chunks_, new_chunks_;
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash>
                                                 loaded_chunks_;
    std::shared_ptr<gfx::rtypes::TextureBinding> tex_;
    const gfx::FlyCam*                           player_cam_;
    std::array<gfx::ShaderHandle,
               static_cast<size_t>(gfx::rtypes::MeshType::kMeshCount)>
        shader_handles_{};
};
};  // namespace pop::voxel
