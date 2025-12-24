#pragma once

#include "core/renderable.hpp"

#include <string>
#include "vertex_buffers.hpp"
#include "texture.hpp"
#include <glm/glm.hpp>

namespace pop::gfx {

class CubeRenderable : public Renderable {
   public:
    CubeRenderable(uint32_t shaderId, const std::string &tex1Path,
                   const std::string &tex2Path)
        : tex1_path_(std::move(tex1Path)),
          tex2_path_(std::move(tex2Path)),
          shader_id_(shaderId) {
        // Vertex data
    }

    void Upload() override {
        // Already uploaded in constructor for now
        float vertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

            -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
            0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

        // VAO/VBO setup
        vao_.Bind();
        vbo_.BufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);
        vao_.AddAttribute({0, 3, GLType::kFloat, false, 5 * sizeof(float), 0});
        vao_.AddAttribute({1, 2, GLType::kFloat, false, 5 * sizeof(float),
                           3 * sizeof(float)});

        // Textures
        tex1_.LoadFromFile(tex1_path_, true);
        tex2_.LoadFromFile(tex2_path_, true);
    }

    void Draw() override {
        tex1_.Bind(0);
        tex2_.Bind(1);

        vao_.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    uint32_t GetShaderProgId() const override { return shader_id_; }

   private:
    VertexArray vao_;
    GLBuffer    vbo_{BufferType::kArrayBuffer};

    Texture tex1_{TextureType::kTexture2D};
    Texture tex2_{TextureType::kTexture2D};

    std::string tex1_path_;
    std::string tex2_path_;
    uint32_t    shader_id_;
};

}  // namespace pop::gfx
