#pragma once

#include "glad/glad.h"
#include <string>
#include <string_view>

namespace pop {
namespace gfx {

enum class ShaderType : GLuint {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER
};

class Shader {
   public:
    explicit Shader(ShaderType stype);
    ~Shader();

    // Loads shader source from file
    static bool LoadShader(Shader& shader, std::string_view fname);

    // Compiles the shader
    bool compile();

    // Returns the shader id
    GLuint id() const { return shader_id_; }

    // Returns last error message (empty if none)
    std::string GetError() const { return error_msg_; }

    // Returns type
    ShaderType type() const { return shader_type_; }

   private:
    ShaderType shader_type_;
    GLuint shader_id_;
    std::string shader_source_;
    std::string error_msg_;
    bool loaded_;

    static constexpr std::string_view kNotLoadedMsg =
        "Load the Shader before compiling it.";
};

}  // namespace gfx
}  // namespace pop
