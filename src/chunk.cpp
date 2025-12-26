#include "voxel/chunk.hpp"
#include "voxel/directions.hpp"
#include "gl/gl_types.hpp"
#include "glad/glad.h"
#include "graphics/rendertypes.hpp"
#include "graphics/shader.hpp"
#include "graphics/vertex_buffers.hpp"
#include "voxel/terrain_generator.hpp"
#include <iostream>
#include <memory>

namespace pop::voxel {

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

// ==============VOXEL==============
Voxel::Voxel(Voxel::Type vtype) : type_(vtype) {}
bool Voxel::IsSolid(Type type) {
    return type != Type::kAir && type != Type::kWater;
}

Voxel::Type Voxel::GetType() const { return type_; }

void Voxel::SetType(Voxel::Type vtype) { type_ = vtype; }

constexpr int VoxelTypeToTexture(const Voxel::Type &voxelType) {
    switch (voxelType) {
        case Voxel::Type::kGrass:
            return 3;
        case Voxel::Type::kDirt:
            return 4;
        case Voxel::Type::kStone:
            return 0;
        case Voxel::Type::kSand:
            return 1;
        default:
            return 0;
    }
}
//==============FaceGeometry==============
constexpr const float *FaceGeometry::GetFace(pop::direction faceDirection) {
    return kFaceTable[static_cast<int>(faceDirection)];
}
// ==============CHUNK===============
Chunk::Chunk(glm::ivec3 chunkOffset) : chunk_offset_(chunkOffset) {
    voxel_data_ = std::make_unique<Voxel[]>(kSize_x * kSize_y * kSize_z);
    PopulateFromHeightMap();
}

Voxel::Type Chunk::GetVoxelAtCoord(const glm::ivec3 &coord) const {
    return voxel_data_[Index(coord.x, coord.y, coord.z)].GetType();
}

constexpr inline int Chunk::Index(int x, int y, int z) {
    return x + kSize_x * (y + kSize_y * z);
}

void Chunk::BreakBlock(const glm::ivec3 &coord) {
    SetVoxelType(Index(coord.x, coord.y, coord.z), Voxel::Type::kAir);
}

void Chunk::AddBlock(const glm::ivec3 &coord, Voxel::Type vtype) {
    SetVoxelType(Index(coord.x, coord.y, coord.z), vtype);
}
void Chunk::SetVoxelType(int index, Voxel::Type vtype) {
    voxel_data_[index].SetType(vtype);
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
    auto  SetBlock = [&](int x, int y, int z, Voxel::Type vtype) {
        voxel_data_[Index(x, y, z)].SetType(vtype);
    };
    for (int x = 0; x < kSize_x; x++) {
        for (int z = 0; z < kSize_z; z++) {
            bool surfaceFound = false;
            for (int y = kSize_y - 1; y >= 0; y--) {
                float density = instance.GetDensity(chunk_offset_.x + x,
                                                    chunk_offset_.y + y,
                                                    chunk_offset_.z + z);
                if (density > 0) {
                    if (!surfaceFound) {
                        if (y >= kWaterBaseline - 1) {
                            SetBlock(
                                x, y, z,
                                Voxel::Type::kGrass);  // The very top layer
                        } else {
                            SetBlock(x, y, z,
                                     Voxel::Type::kSand);  // Underwater floor
                        }
                        surfaceFound = true;
                    } else if (y > (kWaterBaseline - 4) &&
                               y < (kWaterBaseline)) {
                        // Just a little dirt/sand under the surface before
                        // stone starts
                        SetBlock(x, y, z, Voxel::Type::kDirt);
                    } else {
                        SetBlock(x, y, z,
                                 Voxel::Type::kStone);  // Deep underground
                    }
                } else {
                    if (y <= kWaterBaseline) {
                        SetBlock(x, y, z,
                                 Voxel::Type::kWater);  // Fill empty gaps below
                                                        // sea level
                    } else {
                        SetBlock(x, y, z, Voxel::Type::kAir);
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
                auto index = Index(x, y, z);
                auto vtype = voxel_data_[index].GetType();
                if (vtype == Voxel::Type::kAir) continue;
                if (vtype == Voxel::Type::kWater)
                    GenerateVoxel(x, y, z, vtype,
                                  meshes_[MeshToIndex(
                                      gfx::rtypes::MeshType::kWaterMesh)]);
                else
                    GenerateVoxel(x, y, z, vtype,
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
bool Chunk::ShouldDrawFace(Voxel::Type current, Voxel::Type neighbor) const {
    if (neighbor == Voxel::Type::kAir) return true;

    if (current == Voxel::Type::kWater) {
        // Water ONLY draws against Air.
        // It does NOT draw against other water (prevents internal faces)
        // It does NOT draw against solids (solids draw their own face)
        return false;
    } else {
        // Solid draws against Air (standard) and Water (transparency)
        return (neighbor == Voxel::Type::kWater);
    }
}
void Chunk::GenerateVoxel(int x, int y, int z, Voxel::Type vtype,
                          const std::shared_ptr<ChunkRenderable> &mesh) {
    constexpr int floats_per_vertex = 5;
    constexpr int floats_per_face   = floats_per_vertex * 6;

    auto &verts     = mesh->VertexData();
    auto  emit_face = [&](direction dir) {
        const float *face = FaceGeometry::GetFace(dir);

        for (int i = 0; i < floats_per_face; i += 5) {
            verts.push_back(face[i + 0] + x);  // px
            verts.push_back(face[i + 1] + y);  // py
            verts.push_back(face[i + 2] + z);  // pz
            verts.push_back(face[i + 3]);      // u
            verts.push_back(face[i + 4]);      // v
            verts.push_back(kNormalTable[static_cast<int>(dir)]);
            verts.push_back(
                VoxelTypeToTexture(voxel_data_[Index(x, y, z)].GetType()));
        }
    };
    if (ShouldDrawFace(vtype, GetVoxelType(x, y + 1, z)))
        emit_face(direction::kTop);
    if (ShouldDrawFace(vtype, GetVoxelType(x, y - 1, z)))
        emit_face(direction::kBottom);
    if (ShouldDrawFace(vtype, GetVoxelType(x, y, z - 1)))
        emit_face(direction::kNorth);
    if (ShouldDrawFace(vtype, GetVoxelType(x, y, z + 1)))
        emit_face(direction::kSouth);
    if (ShouldDrawFace(vtype, GetVoxelType(x - 1, y, z)))
        emit_face(direction::kWest);
    if (ShouldDrawFace(vtype, GetVoxelType(x + 1, y, z)))
        emit_face(direction::kEast);
}
inline Voxel::Type Chunk::GetLocalVoxelType(int x, int y, int z) const {
    assert(x < kSize_x && y < kSize_y && z < kSize_z &&
           "GetLocalVoxelType out of bounds");
    return voxel_data_[Index(x, y, z)].GetType();
}
Voxel::Type Chunk::GetVoxelType(int x, int y, int z) const {
    // 1. Local Check
    if (x >= 0 && x < kSize_x && y >= 0 && y < kSize_y && z >= 0 &&
        z < kSize_z) {
        return GetLocalVoxelType(x, y, z);
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

    return neighbor ? neighbor->GetLocalVoxelType(nx, ny, nz)
                    : Voxel::Type::kAir;
}
};  // namespace pop::voxel
