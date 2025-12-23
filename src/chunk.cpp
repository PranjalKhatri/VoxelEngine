#include "chunk.hpp"
#include "directions.hpp"
#include "gl_types.hpp"
#include "glad/glad.h"
#include "rendertypes.hpp"
#include "shader.hpp"
#include "vertex_buffers.hpp"
#include "terrain_generator.hpp"
#include <iostream>
#include <memory>

namespace pop::voxel {

// ===============Chunk Renderable==============
ChunkRenderable::ChunkRenderable(gfx::ShaderHandle shaderId)
    : shader_id_{shaderId},
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
            return;  // ❗ do NOT throw
        }
    }
    textures_.emplace_back(std::move(texture));
}
void ChunkRenderable::Upload() {
    if (!vertex_data_ || vertex_data_->empty()) {
        std::cout << "Upload called on empty data\n";
        return;
    }
    if (attributes_.empty()) {
        std::cout << "Attributes not filled\n";
    }

    vao_.Bind();
    vbo_.Bind();

    vbo_.BufferData(vertex_data_->size() * sizeof(float), vertex_data_->data(),
                    GL_STATIC_DRAW);

    for (const auto &attr : attributes_) {
        vao_.AddAttribute(attr);
    }
    vbo_.UnBind();
    vao_.UnBind();
    // std::cout << "Chunk Renderale uploaded " << vertex_data_->size()
    //           << " values; vao_: " << vao_.id() << "\n";
    num_triangles_ = vertex_data_->size() / 6.0;
    // vertex_data_->clear();  // free heap memmory after sending it to gpu
}
void ChunkRenderable::Draw(gfx::ShaderProgram *const shader_program) {
    vao_.Bind();

    for (auto i : textures_) {
        i->texture->Bind(i->slot);
    }
    shader_program->SetUniformFloat3("uChunkOffset", chunk_offset_.x,
                                     chunk_offset_.y, chunk_offset_.z);

    glDrawArrays(GL_TRIANGLES, 0, num_triangles_);
}

// ==============VOXEL==============
Voxel::Voxel(Voxel::Type vtype) : type_(vtype) {}
bool Voxel::IsSolid() const {
    return type_ != Type::kAir && type_ != Type::kWater;
}

Voxel::Type Voxel::GetType() const { return type_; }

void Voxel::SetType(Voxel::Type vtype) { type_ = vtype; }

constexpr int VoxelTypeToTexture(const Voxel::Type &voxelType) {
    switch (voxelType) {
        case Voxel::Type::kGrass:
            return 3;
        case Voxel::Type::kDirt:
            return 2;
        case Voxel::Type::kStone:
            return 6;
        case Voxel::Type::kSand:
            return 5;
        case Voxel::Type::kTreeBark:
            return 4;
        case Voxel::Type::kTreeLeaves:
            return 1;
        default:
            return 100;
    }
}
//==============FaceGeometry==============
constexpr const float *FaceGeometry::GetFace(pop::direction faceDirection) {
    return kFaceTable[static_cast<int>(faceDirection)];
}
// ==============CHUNK===============
Chunk::Chunk(glm::ivec3 chunkOffset)
    : chunk_offset_(chunkOffset), voxel_data_{}, solid_mesh_{} {
    // std::cout << "Chunk created with offset : " << chunkOffset.x << " "
    //           << chunkOffset.y << " " << chunkOffset.z << "\n";
}
Chunk::~Chunk() {}
int Chunk::Index(int x, int y, int z) {
    return x + kSize_x * (y + kSize_y * z);
}

void Chunk::SetShader(gfx::ShaderHandle id) { solid_shader_id_ = id; }
std::shared_ptr<ChunkRenderable> Chunk::GetSolidRenderable() const {
    if (!solid_mesh_) {
        std::cout << "Call to GetSolidRenderable made without construction!\n";
    }
    return solid_mesh_;
}

void Chunk::GenerateMesh() {
    voxel_data_ = std::make_unique<Voxel[]>(kSize_x * kSize_y * kSize_z);
    solid_mesh_ = std::make_shared<ChunkRenderable>(solid_shader_id_);

    PopulateFromHeightMap();
    GenerateRenderable();
}
void Chunk::PopulateFromHeightMap() {
    for (int x = 0; x < kSize_x; x++) {
        for (int z = 0; z < kSize_z; z++) {
            float variableMultiplier =
                terrain::TerrainGenerator::GetInstance().GetHeight(
                    chunk_offset_.x + x, chunk_offset_.z + z);
            int cellHeight = kBaseHeight + variableMultiplier * kVariableHeight;
            for (int y = 0; y < cellHeight; y++) {
                auto index = Index(x, y, z);
                voxel_data_[index].SetType(Voxel::Type::kStone);
            }
        }
    }
}

void Chunk::GenerateRenderable() {
    for (int x = 0; x < kSize_x; x++) {
        for (int y = 0; y < kSize_y; y++) {
            for (int z = 0; z < kSize_z; z++) {
                auto index = Index(x, y, z);
                if (voxel_data_[index].GetType() == Voxel::Type::kAir) continue;
                GenerateVoxel(x, y, z);
            }
        }
    }
    int stride = sizeof(float) * 7;
    solid_mesh_->AddAttribute({0, 3, gfx::GLType::kFloat, false, stride, 0});
    solid_mesh_->AddAttribute(
        {1, 2, gfx::GLType::kFloat, false, stride, 3 * sizeof(float)});
    solid_mesh_->AddAttribute(
        {2, 1, gfx::GLType::kFloat, false, stride, 5 * sizeof(float)});
    solid_mesh_->AddAttribute(
        {3, 1, gfx::GLType::kFloat, false, stride, 6 * sizeof(float)});
    solid_mesh_->SetChunkOffset(chunk_offset_);
}
void Chunk::GenerateVoxel(int x, int y, int z) {
    // For each vertex of the cube
    // vertices[] = { px, py, pz, u, v, ... }
    constexpr int floats_per_vertex = 5;
    constexpr int floats_per_face   = floats_per_vertex * 6;

    auto &verts     = solid_mesh_->VertexData();
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
    if (!IsSolid(x, y + 1, z)) emit_face(direction::kTop);
    if (!IsSolid(x, y - 1, z)) emit_face(direction::kBottom);
    if (!IsSolid(x + 1, y, z)) emit_face(direction::kEast);
    if (!IsSolid(x - 1, y, z)) emit_face(direction::kWest);
    if (!IsSolid(x, y, z - 1)) emit_face(direction::kNorth);
    if (!IsSolid(x, y, z + 1)) emit_face(direction::kSouth);
}

bool Chunk::IsSolid(int x, int y, int z) const {
    // Inside this chunk
    if (x >= 0 && x < kSize_x && y >= 0 && y < kSize_y && z >= 0 &&
        z < kSize_z) {
        return voxel_data_[Index(x, y, z)].IsSolid();
    }

    // TODO: check neighbor chunks
    // Example:
    // if (x < 0) return leftChunk && leftChunk->IsSolid(x + kSize_x, y, z);
    // if (x >= kSize_x) return rightChunk && rightChunk->IsSolid(x - kSize_x,
    // y, z);
    return false;
}
};  // namespace pop::voxel
