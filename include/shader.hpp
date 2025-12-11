#pragma once

#include "glad/glad.h"
#include <string>
#include <string_view>
#include <vector>

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
    static bool LoadShader(Shader& shader, std::string_view path);
    // Helper which prints the error as well load and compiles the shader
    static bool LoadAndCompile(Shader& shader, std::string_view path);

    // Compiles the shader
    bool Compile();

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

class ShaderProgram {
   public:
    explicit ShaderProgram();
    ~ShaderProgram();
    // Attatchs the shader to the program
    void Attach(const Shader& shader);
    // Link all the attatched shaders
    bool Link();
    // starts using this shader program from now on
    void use();
    // Returns last error message (empty if none)
    std::string GetError() const { return error_msg_; }

   private:
    GLuint program_id_;
    std::string error_msg_;
};
}  // namespace gfx
}  // namespace pop
