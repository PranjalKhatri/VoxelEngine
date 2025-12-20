#pragma once

#include "gl_types.hpp"
#include "glm/fwd.hpp"
#include <string>
#include <string_view>

namespace pop::gfx {

class Shader {
   public:
    explicit Shader(ShaderStage stype);
    ~Shader();

    // Loads shader source from file
    static bool LoadShader(Shader& shader, std::string_view path);
    // Helper which prints the error as well load and compiles the shader
    static bool LoadAndCompile(Shader& shader, std::string_view path);

    // Compiles the shader
    bool Compile();

    // Returns the shader id
    ShaderHandle GetHandle() const { return shader_id_; }

    // Returns last error message (empty if none)
    std::string GetError() const { return error_msg_; }

    // Returns type
    ShaderStage type() const { return shader_type_; }

   private:
    ShaderStage  shader_type_{};
    ShaderHandle shader_id_{};
    std::string  shader_source_{};
    std::string  error_msg_{};
    bool         loaded_{};

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
    // Returns the shader id
    ShaderHandle id() const { return program_id_; }
    // Returns last error message (empty if none)
    std::string GetError() const { return error_msg_; }

    void SetUniformBool(std::string_view name, bool value);
    void SetUniformInt(std::string_view name, int value);
    void SetUniformFloat(std::string_view name, float value);
    void SetUniformFloat2(std::string_view name, float x, float y);
    void SetUniformFloat3(std::string_view name, float x, float y, float z);
    void SetUniformFloat4(std::string_view name, float x, float y, float z,
                          float w);
    void SetUniformMat4(std::string_view name, glm::mat4 ptr, bool transpose);

   private:
    ShaderHandle program_id_{};
    std::string  error_msg_{};
};
}  // namespace pop::gfx
