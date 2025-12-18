#pragma once

#include "gl_types.hpp"
#include <glad/glad.h>

namespace pop {
class Renderable {
   public:
    virtual ~Renderable() = default;

    virtual void              Upload()                = 0;
    virtual void              Draw()                  = 0;
    virtual gfx::ShaderHandle GetShaderProgId() const = 0;

   private:
};
}  // namespace pop
