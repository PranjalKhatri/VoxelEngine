#include "shaderLoader.hpp"
#include <fstream>
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

bool Shader::LoadShader(Shader& shader, std::string_view fname) {
    std::ifstream file{std::string(fname)};
    if (!file) {
        shader.error_msg_ = "Failed to open shader file.";
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    shader.shader_source_ = ss.str();
    shader.loaded_ = true;

    return true;
}

bool Shader::compile() {
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

}  // namespace gfx
}  // namespace pop
