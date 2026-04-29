#pragma once
#include "entity_manager.h"
#include "camera.h"

struct GLFWwindow;

// Dear ImGui-based heads-up display overlay.
class HUD {
public:
    HUD(GLFWwindow* window);
    ~HUD();

    void begin_frame();
    void draw(const std::vector<Entity>& entities, const Camera& cam,
              double now, float fps);
    void end_frame();

private:
    void draw_entity_list(const std::vector<Entity>& entities);
    void draw_status_bar(const Camera& cam, float fps);
};
