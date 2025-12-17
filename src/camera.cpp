#include "camera.hpp"

namespace pop::gfx {

// Camera base

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
    : position_{position} {
    back_  = glm::normalize(position_ - target);
    right_ = glm::normalize(glm::cross(up, back_));
    up_    = glm::cross(back_, right_);
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(position_,
                       position_ - back_,  // forward = -back
                       up_);
}

// FlyCam

void FlyCam::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = movement_speed_ * deltaTime;

    if (direction == CameraMovement::Forward) position_ -= back_ * velocity;
    if (direction == CameraMovement::Backward) position_ += back_ * velocity;
    if (direction == CameraMovement::Right) position_ += right_ * velocity;
    if (direction == CameraMovement::Left) position_ -= right_ * velocity;
}

void FlyCam::ProcessMouseMovement(float xpos, float ypos) {
    if (first_mouse_) {
        last_x_      = xpos;
        last_y_      = ypos;
        first_mouse_ = false;
        return;
    }

    float xoffset = xpos - last_x_;
    float yoffset = last_y_ - ypos;

    last_x_            = xpos;
    last_y_            = ypos;
    float sensitivity  = 0.1f;
    xoffset           *= sensitivity;
    yoffset           *= sensitivity;

    yaw_   += xoffset;
    pitch_ += yoffset;

    updateCameraVectors();
}

void FlyCam::updateCameraVectors() {
    // Forward vector (camera looks towards -Z)
    glm::vec3 forward;
    forward.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    forward.y = sin(glm::radians(pitch_));
    forward.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

    back_ = glm::normalize(-forward);

    // Recompute orthonormal basis
    right_ = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), back_));
    up_    = glm::cross(back_, right_);
}

}  // namespace pop::gfx
