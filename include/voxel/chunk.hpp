#pragma once

#include "directions.hpp"
#include "gl/gl_types.hpp"
#include "glm/vec3.hpp"
#include "glm/fwd.hpp"
#include "core/renderable.hpp"
#include "graphics/rendertypes.hpp"
#include "graphics/shader.hpp"
#include "graphics/vertex_buffers.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <vector>
namespace pop::voxel {

class Voxel {
   public:
    enum class Type : uint8_t {
        kAir,
        kGrass,
        kDirt,
        kStone,
        kSand,
        kTreeBark,
        kTreeLeaves,
        kWater
    };

    Voxel(Type type = Type::kAir);

    bool IsSolid() const;
    Type GetType() const;

    void SetType(Type vtype);

   private:
    Type type_;
};

static constexpr int    VoxelTypeToTexture(const Voxel::Type& voxelType);
static constexpr size_t MeshToIndex(gfx::rtypes::MeshType type) {
    return static_cast<size_t>(type);
}

struct FaceGeometry {
    static constexpr int kStride      = 5;
    static constexpr int kVertexCount = 6;

    static constexpr const float* GetFace(pop::direction faceDirection);
};
class ChunkRenderable : public Renderable {
   public:
    ChunkRenderable(gfx::ShaderHandle shaderId, bool isTransparent = false);
    ~ChunkRenderable();

    void              Upload() override;
    void              Draw(gfx::ShaderProgram* const shader_program) override;
    gfx::ShaderHandle GetShaderProgId() const override;
    bool              IsTransparent() const { return is_transparent_; }

    void AddTexture(std::shared_ptr<gfx::rtypes::TextureBinding> texture);
    void AddAttribute(const gfx::Attribute& attribute);
    void AddVertexData(const std::vector<float>& data);
    void SetChunkOffset(const glm::ivec3& offset) { chunk_offset_ = offset; }
    std::vector<float>& VertexData() { return *vertex_data_; }

   private:
    bool              is_transparent_;
    gfx::VertexArray  vao_{true};
    gfx::GLBuffer     vbo_{gfx::BufferType::kArrayBuffer, true};
    gfx::GLBuffer     ebo_{gfx::BufferType::kElementArrayBuffer, true};
    gfx::ShaderHandle shader_id_;
    int               num_triangles_{};
    std::vector<std::shared_ptr<gfx::rtypes::TextureBinding>> textures_;

    glm::ivec3 chunk_offset_{};

    std::unique_ptr<std::vector<float>> vertex_data_;
    std::vector<gfx::Attribute>         attributes_;
};

class Chunk {
   public:
    constexpr static int kSize_x         = 16;
    constexpr static int kBaseHeight     = 20;
    constexpr static int kVariableHeight = 20;
    constexpr static int kSize_y         = kBaseHeight + kVariableHeight;
    constexpr static int kWaterBaseline  = kBaseHeight - 5;
    constexpr static int kSize_z         = 16;
    constexpr static int kNumMeshes =
        static_cast<int>(gfx::rtypes::MeshType::kMeshCount);

    Chunk(glm::ivec3 chunkOffset);
    ~Chunk() = default;

    void GenerateMesh();
    void SetShader(gfx::rtypes::MeshType shaderMeshType,
                   gfx::ShaderHandle     shaderHandle);

    constexpr static int Index(int x, int y, int z);

    std::shared_ptr<ChunkRenderable> GetRenderable(
        gfx::rtypes::MeshType mtype) const;

   private:
    void GenerateVoxel(int x, int y, int z,
                       const std::shared_ptr<ChunkRenderable>& mesh);
    void GenerateRenderable();
    void PopulateFromHeightMap();
    bool IsSolid(int x, int y, int z) const;

   private:
    glm::ivec3 chunk_offset_{};

    std::unique_ptr<Voxel[]> voxel_data_{};

    std::array<gfx::ShaderHandle, kNumMeshes>                shader_ids_{};
    std::array<std::shared_ptr<ChunkRenderable>, kNumMeshes> meshes_{};
};
// 6 vertices * (3 pos + 2 uv) = 30 floats per face
static constexpr float kTopFace[] = {
    // pos              // uv
    0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,

    1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0,
};

static constexpr float kBottomFace[] = {
    0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0,

    1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
};

static constexpr float kNorthFace[] = {
    // -Z
    1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1,

    0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0,
};

static constexpr float kSouthFace[] = {
    // +Z
    0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1,

    1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0,
};

static constexpr float kWestFace[] = {
    // -X
    0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1,

    0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0,
};

static constexpr float kEastFace[] = {
    // +X
    1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1,

    1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0,
};

static constexpr const float*
    kFaceTable[static_cast<size_t>(direction::kCount)] = {
        kTopFace, kBottomFace, kNorthFace, kSouthFace, kWestFace, kEastFace,
};
static constexpr const float kNormalTable[] = {0, 1, 2, 3, 4, 5};
};  // namespace pop::voxel
