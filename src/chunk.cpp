#include "voxel/chunk.hpp"
#include "block/block_ids.hpp"
#include "block/block_registry.hpp"
#include "util/directions.hpp"
#include "gl/gl_types.hpp"
#include "glad/glad.h"
#include "graphics/rendertypes.hpp"
#include "graphics/shader.hpp"
#include "graphics/vertex_buffers.hpp"
#include "voxel/terrain_generator.hpp"
#include <iostream>
#include <memory>

namespace pop::voxel {
using util::direction;
// ===============Chunk Renderable==============
ChunkRenderable::ChunkRenderable(gfx::ShaderHandle shaderId, bool isTransparent)
    : is_transparent_(isTransparent),
      shader_id_{shaderId},
      vertex_data_{std::make_unique<std::vector<float>>()} {}

ChunkRenderable::~ChunkRenderable() {
    // std::cout << "Chunk renderable destructor called, vao_ " << vao_.id()
    //           << "\n";
}
gfx::ShaderHandle ChunkRenderable::GetShaderProgId() const {
    return shader_id_;
}
void ChunkRenderable::AddAttribute(const gfx::Attribute &attribute) {
    attributes_.push_back(attribute);
}
void ChunkRenderable::AddVertexData(const std::vector<float> &data) {
    vertex_data_->insert(vertex_data_->end(), data.begin(), data.end());
}
void ChunkRenderable::AddTexture(
    std::shared_ptr<gfx::rtypes::TextureBinding> texture) {
    for (const auto &i : textures_) {
        if (i->slot == texture->slot) {
            std::cerr << "Texture slot already in use: " << int(texture->slot)
                      << "\n";
            return;
        }
    }
    textures_.emplace_back(std::move(texture));
}
void ChunkRenderable::Upload() {
    if (!vertex_data_ || vertex_data_->empty()) {
        // std::cout << "Upload called on empty data\n";
        return;
    }
    if (attributes_.empty()) {
        std::cout << "Attributes not filled\n";
    }

    vao_.Bind();
    vbo_.Bind();

    vbo_.BufferData(vertex_data_->size() * sizeof(float), vertex_data_->data(),
                    GL_DYNAMIC_DRAW);

    if (first_upload_) {
        for (const auto &attr : attributes_) {
            vao_.AddAttribute(attr);
        }
    }
    vbo_.UnBind();
    vao_.UnBind();
    num_vertices_ = vertex_data_->size() / 7;
    // std::cout << "Chunk Renderale uploaded " << vertex_data_->size()
    //           << " values; vao_: " << vao_.id() << "\n";
    vertex_data_->clear();  // free heap memmory after sending it to gpu
    first_upload_ = false;
}
void ChunkRenderable::Draw(gfx::ShaderProgram *const shader_program) {
    if (!vertex_data_) return;
    vao_.Bind();

    for (auto i : textures_) {
        i->texture->Bind(i->slot);
    }
    shader_program->SetUniformFloat3("uChunkOffset", chunk_offset_.x,
                                     chunk_offset_.y, chunk_offset_.z);

    glDrawArrays(GL_TRIANGLES, 0, num_vertices_);
}

inline int BlockIdToTexture(block::BlockID blockId) {
    if (blockId == block::IDs::GRASS) return 3;
    if (blockId == block::IDs::DIRT) return 4;
    if (blockId == block::IDs::STONE) return 0;
    if (blockId == block::IDs::SAND) return 1;
    return 0;
}
//==============FaceGeometry==============
constexpr const float *FaceGeometry::GetFace(direction faceDirection) {
    return kFaceTable[static_cast<int>(faceDirection)];
}
// ==============CHUNK===============
Chunk::Chunk(glm::ivec3 chunkOffset) : chunk_offset_(chunkOffset) {
    voxel_data_ =
        std::make_unique<block::BlockID[]>(kSize_x * kSize_y * kSize_z);
    PopulateFromHeightMap();
}

block::BlockID Chunk::GetBlockAtCoord(const glm::ivec3 &coord) const {
    return voxel_data_[Index(coord.x, coord.y, coord.z)];
}

constexpr int Chunk::Index(int x, int y, int z) {
    return x + kSize_x * (y + kSize_y * z);
}

void Chunk::BreakBlock(const glm::ivec3 &coord) {
    SetBlockType(Index(coord.x, coord.y, coord.z), block::IDs::AIR);
}

void Chunk::AddBlock(const glm::ivec3 &coord, block::BlockID bid) {
    SetBlockType(Index(coord.x, coord.y, coord.z), bid);
}
void Chunk::SetBlockType(int index, block::BlockID bid) {
    voxel_data_[index] = bid;
}
void Chunk::SetShader(gfx::rtypes::MeshType shaderMeshType,
                      gfx::ShaderHandle     shaderHandle) {
    const size_t index = MeshToIndex(shaderMeshType);
    shader_ids_[index] = shaderHandle;
}
std::shared_ptr<ChunkRenderable> Chunk::GetRenderable(
    gfx::rtypes::MeshType mtype) const {
    const size_t index = MeshToIndex(mtype);
    return meshes_[index];
}

void Chunk::GenerateMesh() {
    for (int i = 0; i < kNumMeshes; i++)
        meshes_[i] = std::make_shared<ChunkRenderable>(
            shader_ids_[i], gfx::rtypes::IsTransparentMesh(
                                static_cast<gfx::rtypes::MeshType>(i)));

    GenerateRenderable();
}
void Chunk::ReGenerate() {
    for (int i = 0; i < kNumMeshes; i++) {
        meshes_[i]->clearData();
    }
    GenerateRenderable();
}
void Chunk::PopulateFromHeightMap() {
    auto &instance = terrain::TerrainGenerator::GetInstance();
    // auto  SetBlock = [&](int x, int y, int z, Voxel::Type vtype) {
    //     voxel_data_[Index(x, y, z)].SetType(vtype);
    // };
    for (int x = 0; x < kSize_x; x++) {
        for (int z = 0; z < kSize_z; z++) {
            bool surfaceFound = false;
            for (int y = kSize_y - 1; y >= 0; y--) {
                auto  index   = Index(x, y, z);
                float density = instance.GetDensity(chunk_offset_.x + x,
                                                    chunk_offset_.y + y,
                                                    chunk_offset_.z + z);
                if (density > 0) {
                    if (!surfaceFound) {
                        if (y >= kWaterBaseline - 1) {
                            SetBlockType(
                                index,
                                block::IDs::GRASS);  // The very top layer
                        } else {
                            SetBlockType(index,
                                         block::IDs::SAND);  // Underwater floor
                        }
                        surfaceFound = true;
                    } else if (y > (kWaterBaseline - 4) &&
                               y < (kWaterBaseline)) {
                        // Just a little dirt/sand under the surface before
                        // stone starts
                        SetBlockType(index, block::IDs::DIRT);
                    } else {
                        SetBlockType(index,
                                     block::IDs::STONE);  // Deep underground
                    }
                } else {
                    if (y <= kWaterBaseline) {
                        SetBlockType(index,
                                     block::IDs::WATER);  // Fill empty gaps
                                                          // below sea level
                    } else {
                        SetBlockType(index, block::IDs::AIR);
                    }
                }
            }
        }
    }
}

void Chunk::GenerateRenderable() {
    for (int x = 0; x < kSize_x; x++) {
        for (int y = 0; y < kSize_y; y++) {
            for (int z = 0; z < kSize_z; z++) {
                auto index   = Index(x, y, z);
                auto blockId = voxel_data_[index];
                if (blockId == block::IDs::AIR) continue;
                if (blockId == block::IDs::WATER)
                    GenerateBlock(x, y, z, blockId,
                                  meshes_[MeshToIndex(
                                      gfx::rtypes::MeshType::kWaterMesh)]);
                else
                    GenerateBlock(x, y, z, blockId,
                                  meshes_[MeshToIndex(
                                      gfx::rtypes::MeshType::kSolidMesh)]);
            }
        }
    }
    int stride = sizeof(float) * 7;
    for (auto &mesh : meshes_) {
        if (!mesh) continue;
        mesh->AddAttribute({0, 3, gfx::GLType::kFloat, false, stride, 0});
        mesh->AddAttribute(
            {1, 2, gfx::GLType::kFloat, false, stride, 3 * sizeof(float)});
        mesh->AddAttribute(
            {2, 1, gfx::GLType::kFloat, false, stride, 5 * sizeof(float)});
        mesh->AddAttribute(
            {3, 1, gfx::GLType::kFloat, false, stride, 6 * sizeof(float)});
        mesh->SetChunkOffset(chunk_offset_);
    }
}
bool Chunk::ShouldDrawFace(block::BlockID current_id,
                           block::BlockID neighbor_id) const {
    // Identical blocks (unless translucent) usually don't draw
    // internal faces
    if (current_id == neighbor_id) return false;

    const auto                  &reg      = block::BlockRegistry::Get();
    [[maybe_unused]] const auto &current  = reg.GetBlock(current_id);
    const auto                  &neighbor = reg.GetBlock(neighbor_id);

    // Air doesn't have faces)
    if (current_id == block::IDs::AIR) return false;

    // If the neighbor is Air (non-solid/translucent), always draw.
    if (!neighbor.IsSolid()) return true;

    // If the neighbor is translucent (like Water or Glass),
    // we should draw our face so it's visible through the neighbor.
    if (neighbor.IsTranslucent()) {
        // Special case: Don't draw water-to-water faces (already handled by id
        // check above)
        return true;
    }

    //  Neighbor is a solid, opaque block. Hide the face.
    return false;
}
void Chunk::GenerateBlock(int x, int y, int z, block::BlockID blockId,
                          const std::shared_ptr<ChunkRenderable> &mesh) {
    constexpr int floats_per_vertex = 5;
    constexpr int floats_per_face   = floats_per_vertex * 6;

    const auto &reg   = block::BlockRegistry::Get();
    const auto &block = reg.GetBlock(blockId);
    auto       &verts = mesh->VertexData();

    auto emit_face = [&](direction dir) {
        const float *face = FaceGeometry::GetFace(dir);

        for (int i = 0; i < floats_per_face; i += 5) {
            verts.push_back(face[i + 0] + x);  // px
            verts.push_back(face[i + 1] + y);  // py
            verts.push_back(face[i + 2] + z);  // pz
            verts.push_back(face[i + 3]);      // u
            verts.push_back(face[i + 4]);      // v
            verts.push_back(kNormalTable[static_cast<int>(dir)]);
            verts.push_back(block.GetTextureCode(dir));
        }
    };
    if (ShouldDrawFace(blockId, GetBlockId(x, y + 1, z)))
        emit_face(direction::kTop);
    if (ShouldDrawFace(blockId, GetBlockId(x, y - 1, z)))
        emit_face(direction::kBottom);
    if (ShouldDrawFace(blockId, GetBlockId(x, y, z - 1)))
        emit_face(direction::kNorth);
    if (ShouldDrawFace(blockId, GetBlockId(x, y, z + 1)))
        emit_face(direction::kSouth);
    if (ShouldDrawFace(blockId, GetBlockId(x - 1, y, z)))
        emit_face(direction::kWest);
    if (ShouldDrawFace(blockId, GetBlockId(x + 1, y, z)))
        emit_face(direction::kEast);
}
block::BlockID Chunk::GetLocalBlockId(int x, int y, int z) const {
    assert(x < kSize_x && y < kSize_y && z < kSize_z &&
           "GetLocalVoxelType out of bounds");
    return voxel_data_[Index(x, y, z)];
}
block::BlockID Chunk::GetBlockId(int x, int y, int z) const {
    // 1. Local Check
    if (x >= 0 && x < kSize_x && y >= 0 && y < kSize_y && z >= 0 &&
        z < kSize_z) {
        return GetLocalBlockId(x, y, z);
    }

    // 2. Neighbor Check (Top, Bottom, North, South, West, East)
    Chunk *neighbor = nullptr;
    int    nx = x, ny = y, nz = z;

    if (z < 0) {
        neighbor = neighbors_[static_cast<int>(direction::kNorth)];
        nz       = kSize_z - 1;
    } else if (z >= kSize_z) {
        neighbor = neighbors_[static_cast<int>(direction::kSouth)];
        nz       = 0;
    } else if (x < 0) {
        neighbor = neighbors_[static_cast<int>(direction::kEast)];
        nx       = kSize_x - 1;
    } else if (x >= kSize_x) {
        neighbor = neighbors_[static_cast<int>(direction::kWest)];
        nx       = 0;
    }

    return neighbor ? neighbor->GetLocalBlockId(nx, ny, nz) : block::IDs::AIR;
}
};  // namespace pop::voxel
