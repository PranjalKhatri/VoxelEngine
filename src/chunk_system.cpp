#include "voxel/chunk_system.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include "graphics/camera.hpp"
#include "core/engine.hpp"
#include "glm/fwd.hpp"
#include "graphics/rendertypes.hpp"
#include "voxel/chunk.hpp"
#include "voxel/directions.hpp"

namespace pop::voxel {
ChunkManager::ChunkManager(const gfx::FlyCam* player_cam_)
    : player_cam_{player_cam_} {
    std::cout << "Manager constructed!!\n";
}

void ChunkManager::SetShader(gfx::rtypes::MeshType meshType,
                             gfx::ShaderHandle     handle) {
    shader_handles_[static_cast<size_t>(meshType)] = handle;
}
void ChunkManager::SetTexture(
    std::shared_ptr<gfx::rtypes::TextureBinding> tex) {
    tex_ = std::move(tex);
}
Chunk* ChunkManager::GetRawChunkPtr(const ChunkCoord& coord) {
    auto it = loaded_chunks_.find(coord);
    return (it != loaded_chunks_.end()) ? it->second.get() : nullptr;
}
void ChunkManager::UploadChunkToEngine(const ChunkCoord& chunkCoord,
                                       core::Engine&     engine) {
    auto& chunk = loaded_chunks_[chunkCoord];

    for (int i = 0; i < static_cast<int>(gfx::rtypes::MeshType::kMeshCount);
         i++) {
        auto renderable =
            chunk->GetRenderable(static_cast<gfx::rtypes::MeshType>(i));
        if (!renderable) continue;
        engine.AddRenderable(renderable);
    }

    // std::cout << "Loaded chunk:" << chunkCoord.x << " " << chunkCoord.z <<
    // "\n";
}

std::unique_ptr<Chunk> ChunkManager::GenerateChunk(
    const ChunkCoord& chunkCoord) {
    auto chunkOffset = ChunkToOffset(chunkCoord);
    auto chunk       = std::make_unique<Chunk>(chunkOffset);
    for (size_t i = 0;
         i < static_cast<size_t>(gfx::rtypes::MeshType::kMeshCount); i++) {
        auto shader = shader_handles_[i];
        if (shader) {
            chunk->SetShader(static_cast<gfx::rtypes::MeshType>(i), shader);
        }
    }
    return chunk;
}

void ChunkManager::LinkAndMesh(const ChunkCoord& coord, core::Engine& engine) {
    auto& chunk = loaded_chunks_[coord];

    // Order: 0:Top, 1:Bottom, 2:North, 3:South, 4:West, 5:East
    Chunk::NeighborArray neighbors;
    neighbors[static_cast<int>(direction::kTop)]    = nullptr;
    neighbors[static_cast<int>(direction::kBottom)] = nullptr;
    neighbors[static_cast<int>(direction::kNorth)] =
        GetRawChunkPtr(coord + ChunkCoord{0, -1});
    neighbors[static_cast<int>(direction::kSouth)] =
        GetRawChunkPtr(coord + ChunkCoord{0, 1});
    neighbors[static_cast<int>(direction::kEast)] =
        GetRawChunkPtr(coord + ChunkCoord{-1, 0});
    neighbors[static_cast<int>(direction::kWest)] =
        GetRawChunkPtr(coord + ChunkCoord{1, 0});
    chunk->SetNeighbors(neighbors);
    chunk->GenerateMesh();

    for (int i = 0; i < static_cast<int>(gfx::rtypes::MeshType::kMeshCount);
         i++) {
        auto renderable =
            chunk->GetRenderable(static_cast<gfx::rtypes::MeshType>(i));
        if (renderable) {
            renderable->AddTexture(tex_);
            engine.AddRenderable(renderable);
        }
    }
}

bool ChunkManager::IsChunkLoaded(const ChunkCoord& chunkCoord) {
    return loaded_chunks_.count(chunkCoord);
}

void ChunkManager::UnLoadChunk(const ChunkCoord& chunkCoord,
                               core::Engine&     engine) {
    assert(loaded_chunks_.count(chunkCoord) &&
           "Unload call on already unloaded chunk");

    for (int i = 0; i < static_cast<int>(gfx::rtypes::MeshType::kMeshCount);
         i++) {
        auto renderable = loaded_chunks_[chunkCoord]->GetRenderable(
            static_cast<gfx::rtypes::MeshType>(i));
        if (!renderable) continue;
        engine.RemoveRenderable(renderable);
    }
}

void ChunkManager::Run(core::Engine& engine) {
    std::cout << "Starting ChunkSystem" << std::endl;
    ChunkCoord lastChunk{INT_MAX, INT_MAX};
    while (engine.IsRunning()) {
        auto currentChunk = WorldToChunkCoord(player_cam_->GetPosition());
        if (currentChunk == lastChunk) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }
        lastChunk = currentChunk;
        std::unordered_set<ChunkCoord, ChunkCoordHash> activeCoords;
        for (int x = -RenderDistance; x <= RenderDistance; x++) {
            for (int z = -RenderDistance; z <= RenderDistance; z++) {
                auto nextChunk =
                    ChunkCoord{currentChunk.x + x, currentChunk.z + z};
                activeCoords.insert(nextChunk);
            }
        }
        for (const auto& coord : activeCoords) {
            if (loaded_chunks_.find(coord) == loaded_chunks_.end()) {
                loaded_chunks_[coord] = GenerateChunk(coord);
            }
        }
        for (const auto& coord : activeCoords) {
            LinkAndMesh(coord, engine);
            UploadChunkToEngine(coord, engine);
        }
        for (auto it = loaded_chunks_.begin(); it != loaded_chunks_.end();) {
            if (activeCoords.find(it->first) == activeCoords.end()) {
                UnLoadChunk(it->first, engine);
                it = loaded_chunks_.erase(it);
            } else {
                ++it;
            }
        }
    }
    std::cout << "ChunkManager stopped!\n";
}

};  // namespace pop::voxel
