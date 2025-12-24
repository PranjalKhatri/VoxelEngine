#pragma once

#include "gl_types.hpp"
#include "shader.hpp"
#include <glad/glad.h>
#include <memory>

namespace pop {
class Renderable {
   public:
    virtual ~Renderable() = default;

    virtual void              Upload()                        = 0;
    virtual void              Draw(gfx::ShaderProgram* const) = 0;
    virtual gfx::ShaderHandle GetShaderProgId() const         = 0;
    virtual bool              IsTransparent() const           = 0;

   private:
};
}  // namespace pop
