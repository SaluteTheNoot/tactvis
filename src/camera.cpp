#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>

Camera::Camera() {}

void Camera::set_mode(CameraMode m) { mode_ = m; }

void Camera::toggle_mode() {
    mode_ = (mode_ == CameraMode::TopDown) ? CameraMode::Perspective : CameraMode::TopDown;
}

glm::mat4 Camera::view() const {
    if (mode_ == CameraMode::TopDown) {
        glm::vec3 eye(pan_.x, zoom_, pan_.y);
        glm::vec3 ctr(pan_.x, 0.f, pan_.y);
        return glm::lookAt(eye, ctr, glm::vec3(0, 0, -1));
    } else {
        float pitch_r = glm::radians(pitch_);
        float yaw_r   = glm::radians(yaw_);
        glm::vec3 dir(
            std::cos(pitch_r) * std::cos(yaw_r),
            std::sin(pitch_r),
            std::cos(pitch_r) * std::sin(yaw_r)
        );
        return glm::lookAt(pos_, pos_ + dir, glm::vec3(0, 1, 0));
    }
}

glm::mat4 Camera::projection(float aspect) const {
    if (mode_ == CameraMode::TopDown)
        return glm::ortho(-zoom_ * aspect, zoom_ * aspect, -zoom_, zoom_, 1.f, 10000.f);
    else
        return glm::perspective(glm::radians(fov_), aspect, 0.5f, 20000.f);
}

void Camera::process_keyboard(int key, float dt) {
    float speed = zoom_ * 0.8f * dt;
    if (mode_ == CameraMode::TopDown) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP)    pan_.y -= speed;
        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)  pan_.y += speed;
        if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)  pan_.x -= speed;
        if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) pan_.x += speed;
    } else {
        float pitch_r = glm::radians(pitch_);
        float yaw_r   = glm::radians(yaw_);
        glm::vec3 fwd(std::cos(pitch_r)*std::cos(yaw_r), 0, std::cos(pitch_r)*std::sin(yaw_r));
        glm::vec3 right = glm::cross(fwd, glm::vec3(0,1,0));
        float mv = 200.f * dt;
        if (key == GLFW_KEY_W || key == GLFW_KEY_UP)    pos_ += fwd   * mv;
        if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)  pos_ -= fwd   * mv;
        if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)  pos_ -= right * mv;
        if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) pos_ += right * mv;
    }
}

void Camera::process_mouse_drag(float dx, float dy) {
    if (mode_ == CameraMode::TopDown) {
        float scale = zoom_ * 0.002f;
        pan_.x -= dx * scale;
        pan_.y -= dy * scale;
    } else {
        yaw_   += dx * 0.3f;
        pitch_ -= dy * 0.3f;
        pitch_  = std::clamp(pitch_, -89.f, 89.f);
    }
}

void Camera::process_scroll(float dy) {
    if (mode_ == CameraMode::TopDown) {
        zoom_ *= (dy > 0 ? 0.85f : 1.15f);
        zoom_  = std::clamp(zoom_, 50.f, 5000.f);
    } else {
        fov_ -= dy * 2.f;
        fov_  = std::clamp(fov_, 15.f, 90.f);
    }
}
