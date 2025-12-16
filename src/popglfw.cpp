#include "popglfw.hpp"

namespace pop::GLFW {
void init(unsigned int versionMajor, unsigned int versionMinor,
          GLFW_PROFILE profile) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, static_cast<int>(profile));
}
}  // namespace pop::GLFW
