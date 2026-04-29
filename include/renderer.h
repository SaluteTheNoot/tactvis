#pragma once
#include "entity_manager.h"
#include "camera.h"
#include "shader.h"
#include "terrain.h"
#include <memory>

struct GLFWwindow;

class Renderer {
public:
    Renderer(GLFWwindow* window, EntityManager& em);
    ~Renderer();

    void draw(double now, float aspect);

    Camera& camera() { return camera_; }

private:
    void draw_terrain();
    void draw_entities(const std::vector<Entity>& entities, double now);
    void draw_threat_radius(const Entity& e, double now);

    GLFWwindow*    window_;
    EntityManager& em_;
    Camera         camera_;

    std::unique_ptr<Shader>  entity_shader_;
    std::unique_ptr<Shader>  terrain_shader_;
    std::unique_ptr<Shader>  threat_shader_;

    std::unique_ptr<Terrain> terrain_;

    GLuint billboard_vao_{0}, billboard_vbo_{0};
    GLuint circle_vao_{0},    circle_vbo_{0};

    double start_time_;

    void init_billboard_geometry();
    void init_circle_geometry(int segments = 64);
};
