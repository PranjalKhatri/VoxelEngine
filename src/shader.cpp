#include "shader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace pop {
namespace gfx {

Shader::Shader(ShaderType stype)
    : shader_type_{stype},
      shader_id_{glCreateShader(static_cast<GLuint>(stype))},
      loaded_{false} {}

Shader::~Shader() {
    if (shader_id_) {
        glDeleteShader(shader_id_);
    }
}

bool Shader::LoadShader(Shader& shader, std::string_view path) {
    std::ifstream file{std::string(path)};
    if (!file) {
        shader.error_msg_ = "Failed to open shader file.";
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    shader.shader_source_ = ss.str();
    shader.loaded_        = true;

    return true;
}

bool Shader::LoadAndCompile(Shader& shader, std::string_view path) {
    if (!LoadShader(shader, path)) {
        std::cerr << shader.GetError() << "\n";
        return false;
    }
    if (!shader.Compile()) {
        std::cerr << shader.GetError() << "\n";
        return false;
    }
    return true;
}

bool Shader::Compile() {
    if (!loaded_) {
        error_msg_ = std::string(kNotLoadedMsg);
        return false;
    }

    const char* src = shader_source_.c_str();
    glShaderSource(shader_id_, 1, &src, nullptr);
    glCompileShader(shader_id_);

    GLint success = 0;
    glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint logLength = 0;
        glGetShaderiv(shader_id_, GL_INFO_LOG_LENGTH, &logLength);

        error_msg_.resize(logLength);
        glGetShaderInfoLog(shader_id_, logLength, nullptr, error_msg_.data());
        return false;
    }

    error_msg_.clear();
    return true;
}

ShaderProgram::ShaderProgram() : program_id_{glCreateProgram()} {}

ShaderProgram::~ShaderProgram() {
    if (program_id_) {
        glDeleteProgram(program_id_);
    }
}

void ShaderProgram::Attach(const Shader& shader) {
    glAttachShader(program_id_, shader.id());
}

bool ShaderProgram::Link() {
    glLinkProgram(program_id_);

    GLint success = 0;
    glGetProgramiv(program_id_, GL_LINK_STATUS, &success);

    if (!success) {
        GLint logLength;
        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &logLength);

        error_msg_.resize(logLength);
        glGetProgramInfoLog(program_id_, logLength, nullptr, error_msg_.data());
        return false;
    }
    error_msg_.clear();
    return true;
}

void ShaderProgram::use() {
    if (program_id_) glUseProgram(program_id_);
}
}  // namespace gfx
}  // namespace pop
