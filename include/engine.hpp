#pragma once

#include "camera.hpp"
#include "renderable.hpp"
// #include "resource_manager.hpp"

#include <memory>
#include <unordered_set>

#include "GLFW/glfw3.h"
#include "shader.hpp"

#include "safe_queue.hpp"

namespace pop {
enum class RenderCmdType { Add, Remove };

struct RenderCmd {
    RenderCmdType               type;
    std::shared_ptr<Renderable> renderable;
};
class Engine {
   public:
    Engine(glm::mat4 projectionMatrix, GLFWwindow* window);
    ~Engine() = default;

    Engine(const Engine&)            = delete;
    Engine& operator=(const Engine&) = delete;

    Engine(Engine&&)            = delete;
    Engine& operator=(Engine&&) = delete;

    void SetMainCamera(std::shared_ptr<gfx::Camera> camera);

    void AddRenderable(std::shared_ptr<Renderable> renderable);
    void RemoveRenderable(std::shared_ptr<Renderable> renderable);

    // void AddRenderable(const std::shared_ptr<Renderable>& renderable);
    // void RemoveRenderable(const std::shared_ptr<Renderable>& renderable);

    // gfx::ResourceManager&       Resources();
    // const gfx::ResourceManager& Resources() const;
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
    void ProcessCommands();

   private:
    float last_frame_time_{};
    float delta_time_{};
    bool  is_running_{};

    CmdQueue<RenderCmd> cmd_queue_;

    GLFWwindow* window_{};

    std::unordered_set<std::shared_ptr<Renderable>> solid_renderables_{},
        transparent_renderables_;
    std::unordered_map<gfx::ShaderHandle, std::unique_ptr<gfx::ShaderProgram>>
                                 shader_prog_map_{};
    std::shared_ptr<gfx::Camera> main_camera_{};
    glm::mat4                    projection_matrix_{};

    // gfx::ResourceManager resource_manager_;
};
};  // namespace pop
