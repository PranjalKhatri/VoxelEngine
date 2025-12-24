#include "glad/glad.h"
#include "engine.hpp"
#include "camera.hpp"
#include "glm/fwd.hpp"
#include "shader.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>

namespace pop {
Engine::Engine(glm::mat4 projectionMatrix, GLFWwindow *window)
    : is_running_{false},
      cmd_queue_{},
      window_(window),
      solid_renderables_{},
      projection_matrix_{projectionMatrix} {
    glfwSetWindowUserPointer(window_, this);
    glfwSetCursorPosCallback(window_, MouseCallback);
    glfwSetFramebufferSizeCallback(window_, FramebufferSizeCallback);
    is_running_ = true;
}

bool Engine::IsRunning() const { return is_running_; }

float Engine::GetDeltaTime() const { return delta_time_; }

void Engine::AddShaderProgram(std::unique_ptr<gfx::ShaderProgram> shaderProg) {
    shader_prog_map_[shaderProg->id()] = std::move(shaderProg);
}
void Engine::AddRenderable(std::shared_ptr<Renderable> renderable) {
    cmd_queue_.push({RenderCmdType::Add, std::move(renderable)});
}
void Engine::RemoveRenderable(std::shared_ptr<Renderable> renderable) {
    cmd_queue_.push({RenderCmdType::Remove, std::move(renderable)});
}
void Engine::SetMainCamera(std::shared_ptr<gfx::Camera> cam) {
    main_camera_ = std::move(cam);
}
void Engine::ProcessCommands() {
    // TODO: compare performance by limiting number of commands processed per
    // turn
    while (true) {
        auto cmd = cmd_queue_.try_pop();
        if (!cmd) break;
        switch (cmd->type) {
            case RenderCmdType::Add:
                std::cout << "Added renderable\n";
                cmd->renderable->Upload();
                if (cmd->renderable->IsTransparent())
                    transparent_renderables_.insert(std::move(cmd->renderable));
                else
                    solid_renderables_.insert(std::move(cmd->renderable));
                break;
            case RenderCmdType::Remove:
                std::cout << "removed renderable\n";
                if (cmd->renderable->IsTransparent())
                    transparent_renderables_.erase(std::move(cmd->renderable));
                else
                    solid_renderables_.erase(std::move(cmd->renderable));
                break;
        }
    }
}

void Engine::Run() {
    std::cout << "Inside Run" << std::endl;
    if (!main_camera_) {
        throw std::runtime_error("No main Camera attatched to Engine!");
    }
    for (auto &renderable : solid_renderables_) {
        renderable->Upload();
    }
    for (auto &prog : shader_prog_map_) {
        prog.second->use();
        prog.second->SetUniformMat4("projection", projection_matrix_, false);
        // Direction: x=-0.2 (slightly from left), y=-1.0 (mostly down), z=-0.3
        // (slightly from front)
        prog.second->SetUniformFloat3("SunLight.direction", 0.0f, -1.0f, 0.0f);
        // Ambient: The base light in the shadows (soft blueish gray to simulate
        // sky reflection)
        prog.second->SetUniformFloat3("SunLight.ambient", 0.3f, 0.3f, 0.35f);
        // Diffuse: The direct sun color (slightly warm/yellowish)
        prog.second->SetUniformFloat3("SunLight.diffuse", 0.8f, 0.8f, 0.7f);
    }
    std::cout << "Run init successful\n";
    while (!glfwWindowShouldClose(window_)) {
        ProcessCommands();

        UpdateDeltaTime();
        ProcessInput();

        Render();

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
    is_running_ = false;
}

void Engine::Render() {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto viewMatrix = main_camera_->GetViewMatrix();

    // PASS 1: SOLID RENDERABLES
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    for (auto &drawable : solid_renderables_) {
        auto current_shader_id = drawable->GetShaderProgId();
        if (current_shader_id && shader_prog_map_[current_shader_id]) {
            shader_prog_map_[current_shader_id]->use();
            shader_prog_map_[current_shader_id]->SetUniformMat4(
                "view", viewMatrix, false);

            drawable->Draw(shader_prog_map_[current_shader_id].get());
        }
    }
    // PASS 2: TRANSPARENT RENDERABLES (WATER)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    for (auto &drawable : transparent_renderables_) {
        auto current_shader_id = drawable->GetShaderProgId();
        if (current_shader_id && shader_prog_map_[current_shader_id]) {
            shader_prog_map_[current_shader_id]->use();
            shader_prog_map_[current_shader_id]->SetUniformMat4(
                "view", viewMatrix, false);

            drawable->Draw(shader_prog_map_[current_shader_id].get());
        }
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Engine::FramebufferSizeCallback(GLFWwindow *window, int width,
                                     int height) {
    glViewport(0, 0, width, height);
}
void Engine::UpdateDeltaTime() {
    float currentTime = glfwGetTime();
    delta_time_       = currentTime - last_frame_time_;
    last_frame_time_  = currentTime;
}

void Engine::MouseCallback(GLFWwindow *window, double xpos, double ypos) {
    auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine || !engine->main_camera_) return;

    engine->main_camera_->ProcessMouseMovement(static_cast<float>(xpos),
                                               static_cast<float>(ypos));
}
void Engine::ProcessInput() {
    if (!main_camera_) return;
    auto window = window_;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        main_camera_->ProcessKeyboard(gfx::Camera::CameraMovement::Forward,
                                      delta_time_);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        main_camera_->ProcessKeyboard(gfx::Camera::CameraMovement::Backward,
                                      delta_time_);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        main_camera_->ProcessKeyboard(gfx::Camera::CameraMovement::Left,
                                      delta_time_);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        main_camera_->ProcessKeyboard(gfx::Camera::CameraMovement::Right,
                                      delta_time_);
}
}  // namespace pop
