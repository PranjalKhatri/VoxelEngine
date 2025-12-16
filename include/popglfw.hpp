#pragma once

#include "GLFW/glfw3.h"

namespace pop::GLFW {
enum class GLFW_PROFILE {
    kAnyProfile    = GLFW_OPENGL_ANY_PROFILE,
    kCoreProfile   = GLFW_OPENGL_CORE_PROFILE,
    kCompatProfile = GLFW_OPENGL_COMPAT_PROFILE,
};
void init(unsigned int versionMajor, unsigned int versionMinor,
          GLFW_PROFILE profile);
}  // namespace pop::GLFW
