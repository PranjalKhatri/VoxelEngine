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
        renderable->AddTexture(tex_);
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
void ChunkManager::LinkChunkNeighbors(const ChunkCoord& coord) {
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
}
void ChunkManager::LinkAndMesh(const ChunkCoord& coord, core::Engine& engine) {
    LinkChunkNeighbors(coord);
    auto& chunk = loaded_chunks_[coord];
    chunk->GenerateMesh();
    UploadChunkToEngine(coord, engine);
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
    InitialLoad(engine);
    ChunkCoord lastChunk = WorldToChunkCoord(player_cam_->GetPosition());
    while (engine.IsRunning()) {
        auto currentChunk = WorldToChunkCoord(player_cam_->GetPosition());
        if (currentChunk == lastChunk) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }
        lastChunk = currentChunk;
        std::unordered_set<ChunkCoord, ChunkCoordHash> dirtyChunks,
            chunksToUnload, newChunks;
        for (const auto& i : loaded_chunks_) chunksToUnload.insert(i.first);

        for (int x = -RenderDistance; x <= RenderDistance; x++) {
            for (int z = -RenderDistance; z <= RenderDistance; z++) {
                auto nextChunk = currentChunk + ChunkCoord{x, z};
                if (chunksToUnload.count(nextChunk)) {
                    chunksToUnload.erase(nextChunk);
                } else {
                    newChunks.insert(nextChunk);
                    loaded_chunks_[nextChunk] = GenerateChunk(nextChunk);

                    dirtyChunks.insert(nextChunk);
                    // Its neighbors also need to update their borders
                    ChunkCoord neighbors[] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
                    for (auto& off : neighbors) {
                        ChunkCoord nPos = nextChunk + off;
                        // Only dirty the neighbor if it actually exists in our
                        // system
                        if (loaded_chunks_.count(nPos)) {
                            dirtyChunks.insert(nPos);
                        }
                    }
                }
            }
        }
        // unload out of render distance chunks first so that GetRawptr in the
        // linkneighbor is valid.
        for (const auto& i : chunksToUnload) {
            UnLoadChunk(i, engine);
            loaded_chunks_.erase(i);
        }
        // regenerate dirty chunk and upload again
        for (const auto& dirtyCoord : dirtyChunks) {
            if (loaded_chunks_.find(dirtyCoord) == loaded_chunks_.end())
                continue;

            if (newChunks.find(dirtyCoord) == newChunks.end()) {
                UnLoadChunk(dirtyCoord, engine);
                LinkChunkNeighbors(dirtyCoord);
                loaded_chunks_[dirtyCoord]->ReGenerate();
                UploadChunkToEngine(dirtyCoord, engine);
            } else {
                LinkAndMesh(dirtyCoord, engine);
            }
        }
    }
    std::cout << "ChunkManager stopped!\n";
}
void ChunkManager::InitialLoad(core::Engine& engine) {
    auto currentChunk = WorldToChunkCoord(player_cam_->GetPosition());
    std::unordered_set<ChunkCoord, ChunkCoordHash> activeCoords;
    for (int x = -RenderDistance; x <= RenderDistance; x++) {
        for (int z = -RenderDistance; z <= RenderDistance; z++) {
            auto nextChunk = ChunkCoord{currentChunk.x + x, currentChunk.z + z};
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

};  // namespace pop::voxel
