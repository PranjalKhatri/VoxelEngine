#pragma once

#include "camera.hpp"
#include "renderable.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

#include "GLFW/glfw3.h"
#include "shader.hpp"

namespace pop {
class Engine {
   public:
    Engine(glm::mat4 projectionMatrix, GLFWwindow* window);
    ~Engine() = default;

    Engine(const Engine&)            = delete;
    Engine& operator=(const Engine&) = delete;

    // todo define move ctor

    void SetMainCamera(std::shared_ptr<gfx::Camera> camera);
    void AddRenderable(std::shared_ptr<Renderable> renderable);
    void AddShaderProgram(std::unique_ptr<gfx::ShaderProgram> prog);

    void Run();

    float GetDeltaTime() const;
    bool  IsRunning() const;

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void FramebufferSizeCallback(GLFWwindow* window, int width,
                                        int height);

   private:
    void Render();
    void UpdateDeltaTime();
    void ProcessInput();

   private:
    float last_frame_time_;
    float delta_time_;
    bool  is_running_;

    GLFWwindow* window_;

    std::vector<std::shared_ptr<Renderable>> renderables_;
    std::unordered_map<gfx::ShaderHandle, std::unique_ptr<gfx::ShaderProgram>>
                                 shader_prog_map_;
    std::shared_ptr<gfx::Camera> main_camera_;
    glm::mat4                    projection_matrix_;
};
};  // namespace pop
