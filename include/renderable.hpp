#pragma once

#include <glad/glad.h>

namespace pop {
class Engine;
class Renderable {
   public:
    virtual ~Renderable() = default;

    virtual void     Upload()                = 0;
    virtual void     Draw()                  = 0;
    virtual uint32_t GetShaderProgId() const = 0;

   private:
};
}  // namespace pop
