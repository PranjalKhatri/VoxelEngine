#pragma once

#include "gl_types.hpp"
#include "renderable.hpp"
#include "vertex_buffers.hpp"
#include <cstdint>
#include <memory>
#include <vector>
namespace pop::voxel {

class Voxel {
   public:
    enum class Type : uint8_t { kAir, kGrass, kDirt, kWater };

    Voxel(Type type = Type::kAir);

    bool IsSolid() const;
    Type GetType() const;

    void SetType(Type vtype);

   private:
    Type type_;
};

class ChunkRenderable : public Renderable {
   public:
    ChunkRenderable(gfx::ShaderHandle shaderId);
    ~ChunkRenderable() = default;

    void              Upload() override;
    void              Draw() override;
    gfx::ShaderHandle GetShaderProgId() const override;

    void AddAttribute(const gfx::Attribute& attribute);
    void AddVertexData(const std::vector<float>& data);

   private:
    gfx::VertexArray  vao_;
    gfx::GLBuffer     vbo_{gfx::BufferType::kArrayBuffer};
    gfx::GLBuffer     ebo_{gfx::BufferType::kElementArrayBuffer};
    gfx::ShaderHandle shader_id_;

    std::unique_ptr<std::vector<float>> vertex_data_;
    std::vector<gfx::Attribute>         attributes_;
};

class Chunk {
   public:
    constexpr static int kSize_x = 16;
    constexpr static int kSize_y = 16;
    constexpr static int kSize_z = 16;

    Chunk();
    ~Chunk();

    void GenerateMesh();
    void SetShader(gfx::ShaderHandle id);

    int Index(int x, int y, int z) const;

    std::shared_ptr<ChunkRenderable> GetSolidRenderable() const;

   private:
    void GenerateVoxel(int x, int y, int z);

   private:
    gfx::ShaderHandle                solid_shader_id_;
    std::unique_ptr<Voxel[]>         voxel_data_;
    std::shared_ptr<ChunkRenderable> solid_mesh_;
};

static float vertices[] = {
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  -1.0f, -1.0f, 1.0f, 0.0f,
    1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, 1.0f,  1.0f,  -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,

    -1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  -1.0f, 1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f, 1.0f,  1.0f,  0.0f, 1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0.0f,

    -1.0f, 1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,  0.0f, 0.0f, -1.0f, 1.0f,  1.0f,  1.0f, 0.0f,

    1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  1.0f,  -1.0f, 1.0f, 1.0f,
    1.0f,  -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, -1.0f, 0.0f, 1.0f,
    1.0f,  -1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,

    -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, -1.0f, 1.0f, 1.0f,
    1.0f,  -1.0f, 1.0f,  1.0f, 0.0f, 1.0f,  -1.0f, 1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 1.0f,  0.0f, 0.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f,

    -1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,  1.0f,  -1.0f, 1.0f, 1.0f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
    -1.0f, 1.0f,  1.0f,  0.0f, 0.0f, -1.0f, 1.0f,  -1.0f, 0.0f, 1.0f};

};  // namespace pop::voxel
