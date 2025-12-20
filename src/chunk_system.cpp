#include "chunk_system.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include "camera.hpp"
#include "engine.hpp"
#include "glm/fwd.hpp"

namespace pop::voxel {
ChunkManager::ChunkManager(const gfx::FlyCam* player_cam_)
    : player_cam_{player_cam_} {
    std::cout << "Manager constructed!!\n";
}

void ChunkManager::SetShader(gfx::ShaderHandle handle_) {
    shader_handle_ = handle_;
    std::cout << "Shader set in chunk manager" << std::endl;
}
void ChunkManager::SetTexture(
    std::shared_ptr<gfx::rtypes::TextureBinding> tex) {
    tex_ = std::move(tex);
}
void ChunkManager::LoadChunk(const ChunkCoord& chunkCoord, Engine& engine) {
    auto chunk = GenerateChunk(chunkCoord);

    auto renderable = chunk->GetSolidRenderable();
    renderable->AddTexture(tex_);

    engine.AddRenderable(chunk->GetSolidRenderable());
    loaded_chunks_[chunkCoord] = std::move(chunk);
    std::cout << "Loaded chunk:" << chunkCoord.x << " " << chunkCoord.z << "\n";
}

std::unique_ptr<Chunk> ChunkManager::GenerateChunk(
    const ChunkCoord& chunkCoord) {
    auto chunkOffset = ChunkToOffset(chunkCoord);
    auto chunk       = std::make_unique<Chunk>(chunkOffset);
    chunk->SetShader(shader_handle_);
    chunk->GenerateMesh();
    return chunk;
}

bool ChunkManager::IsChunkLoaded(const ChunkCoord& chunkCoord) {
    return loaded_chunks_.count(chunkCoord);
}

void ChunkManager::UnLoadChunk(const ChunkCoord& chunkCoord, Engine& engine) {
    assert(loaded_chunks_.count(chunkCoord) &&
           "Unload call on already unloaded chunk");
    engine.RemoveRenderable(loaded_chunks_[chunkCoord]->GetSolidRenderable());
    loaded_chunks_.erase(chunkCoord);
}

void ChunkManager::Run(Engine& engine) {
    std::cout << "Starting ChunkSystem" << std::endl;
    ChunkCoord lastChunk{INT_MAX, INT_MAX};
    while (engine.IsRunning()) {
        auto playerPosition = player_cam_->GetPosition();
        auto currentChunk   = WorldToChunkCoord(playerPosition);
        if (currentChunk == lastChunk) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }
        lastChunk = currentChunk;
        std::unordered_set<ChunkCoord, ChunkCoordHash> loadedIndices;
        for (auto& i : loaded_chunks_) loadedIndices.insert(i.first);
        for (int x = -RenderDistance; x <= RenderDistance; x++) {
            for (int z = -RenderDistance; z <= RenderDistance; z++) {
                auto nextChunk  = currentChunk;
                nextChunk.x    += x;
                nextChunk.z    += z;
                if (!IsChunkLoaded(nextChunk)) {
                    LoadChunk(nextChunk, engine);
                }
                loadedIndices.erase(nextChunk);
            }
        }
        for (auto i : loadedIndices) {
            UnLoadChunk(i, engine);
        }
        loadedIndices.clear();
    }
    std::cout << "ChunkManager stopped!\n";
}

};  // namespace pop::voxel
