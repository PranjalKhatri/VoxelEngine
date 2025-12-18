#include "chunk.hpp"
#include "gl_types.hpp"
#include "glad/glad.h"
#include "vertex_buffers.hpp"
#include <iostream>
#include <memory>

namespace pop::voxel {

ChunkRenderable::ChunkRenderable(gfx::ShaderHandle shaderId)
    : shader_id_{shaderId} {
    vertex_data_ = std::make_unique<std::vector<float>>();
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
void ChunkRenderable::Upload() {
    if (!vertex_data_ || vertex_data_->empty()) {
        return;
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
    std::cout << "Chunk Renderale uploaded " << vertex_data_->size()
              << " values\n";
}
void ChunkRenderable::Draw() {
    vao_.Bind();
    glDrawArrays(GL_TRIANGLES, 0, vertex_data_->size() / 5);
}

// ==============VOXEL==============
Voxel::Voxel(Voxel::Type vtype) : type_(vtype) {}
bool Voxel::IsSolid() const {
    return type_ != Type::kAir && type_ != Type::kWater;
}

Voxel::Type Voxel::GetType() const { return type_; }

void Voxel::SetType(Voxel::Type vtype) { type_ = vtype; }

// ==============CHUNK==============
Chunk::Chunk() : solid_mesh_{}, voxel_data_{} {}
Chunk::~Chunk() {}
int Chunk::Index(int x, int y, int z) const {
    return x + kSize_x * (y + kSize_y * z);
}

void Chunk::SetShader(gfx::ShaderHandle id) { solid_shader_id_ = id; }
std::shared_ptr<ChunkRenderable> Chunk::GetSolidRenderable() const {
    return solid_mesh_;
}

void Chunk::GenerateMesh() {
    voxel_data_ = std::make_unique<Voxel[]>(kSize_x * kSize_y * kSize_z);

    for (int i = 0; i < kSize_x * kSize_y * kSize_z; i++) {
        voxel_data_[i].SetType(Voxel::Type::kGrass);
    }
    solid_mesh_ = std::make_shared<ChunkRenderable>(solid_shader_id_);
    std::cout << "space reserved for solid mesh and voxel data\n";
    for (int x = 0; x < kSize_x; x++) {
        for (int y = 0; y < kSize_y; y++) {
            for (int z = 0; z < kSize_z; z++) {
                auto index = Index(x, y, z);
                if (voxel_data_[index].GetType() == Voxel::Type::kAir) continue;
                GenerateVoxel(x, y, z);
            }
        }
    }
    solid_mesh_->AddAttribute(
        {0, 3, gfx::GLType::kFloat, false, 5 * sizeof(float), 0});
    solid_mesh_->AddAttribute({1, 2, gfx::GLType::kFloat, false,
                               5 * sizeof(float), 3 * sizeof(float)});
    std::cout << "Chunk solid mesh genertion done!\n";
}

void Chunk::GenerateVoxel(int x, int y, int z) {
    // For each vertex of the cube
    // vertices[] = { px, py, pz, u, v, ... }
    constexpr int floats_per_vertex = 5;
    constexpr int total_floats      = sizeof(vertices) / sizeof(float);

    std::vector<float> voxel_vertices;
    voxel_vertices.reserve(total_floats);

    for (int i = 0; i < total_floats; i += floats_per_vertex) {
        float px = vertices[i + 0] + static_cast<float>(x);
        float py = vertices[i + 1] + static_cast<float>(y);
        float pz = vertices[i + 2] + static_cast<float>(z);

        float u = vertices[i + 3];
        float v = vertices[i + 4];

        voxel_vertices.push_back(px);
        voxel_vertices.push_back(py);
        voxel_vertices.push_back(pz);
        voxel_vertices.push_back(u);
        voxel_vertices.push_back(v);
        // std::cout << "pushed back " << i << "\n";
    }

    solid_mesh_->AddVertexData(voxel_vertices);
}

};  // namespace pop::voxel
