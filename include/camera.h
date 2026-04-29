#pragma once
#include <glm/glm.hpp>

enum class CameraMode { TopDown, Perspective };

class Camera {
public:
    Camera();

    void set_mode(CameraMode m);
    void toggle_mode();
    CameraMode mode() const { return mode_; }

    glm::mat4 view() const;
    glm::mat4 projection(float aspect) const;

    // Mouse/keyboard input handlers
    void process_keyboard(int key, float dt);
    void process_mouse_drag(float dx, float dy);
    void process_scroll(float dy);

private:
    CameraMode mode_{CameraMode::TopDown};

    // Top-down
    glm::vec2 pan_{0.f, 0.f};
    float     zoom_{1000.f}; // meters visible half-width

    // Perspective
    glm::vec3 pos_{0.f, 200.f, 600.f};
    float     pitch_{-20.f};
    float     yaw_{-90.f};
    float     fov_{60.f};
};
