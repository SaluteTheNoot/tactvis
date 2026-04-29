#include "renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <GLFW/glfw3.h>

static constexpr glm::vec3 COLOR_FRIENDLY{0.0f, 0.9f, 0.3f};
static constexpr glm::vec3 COLOR_HOSTILE {0.9f, 0.1f, 0.1f};
static constexpr glm::vec3 COLOR_UNKNOWN {0.9f, 0.7f, 0.1f};

Renderer::Renderer(GLFWwindow* window, EntityManager& em)
    : window_(window), em_(em), start_time_(glfwGetTime())
{
    entity_shader_  = std::make_unique<Shader>("shaders/entity.vert",  "shaders/entity.frag");
    terrain_shader_ = std::make_unique<Shader>("shaders/terrain.vert", "shaders/terrain.frag");
    threat_shader_  = std::make_unique<Shader>("shaders/threat.vert",  "shaders/threat.frag");

    terrain_ = std::make_unique<Terrain>(128, 4000.f);

    init_billboard_geometry();
    init_circle_geometry();
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &billboard_vao_); glDeleteBuffers(1, &billboard_vbo_);
    glDeleteVertexArrays(1, &circle_vao_);    glDeleteBuffers(1, &circle_vbo_);
}

void Renderer::init_billboard_geometry() {
    float verts[] = {
        -0.5f, 0.f, -0.5f,
         0.5f, 0.f, -0.5f,
         0.5f, 0.f,  0.5f,
        -0.5f, 0.f,  0.5f,
    };
    glGenVertexArrays(1, &billboard_vao_);
    glGenBuffers(1, &billboard_vbo_);
    glBindVertexArray(billboard_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
    glBindVertexArray(0);
}

void Renderer::init_circle_geometry(int segments) {
    std::vector<float> verts;
    verts.reserve(segments * 3);
    for (int i = 0; i < segments; ++i) {
        float angle = 2.f * glm::pi<float>() * i / segments;
        verts.push_back(std::cos(angle));
        verts.push_back(0.f);
        verts.push_back(std::sin(angle));
    }
    glGenVertexArrays(1, &circle_vao_);
    glGenBuffers(1, &circle_vbo_);
    glBindVertexArray(circle_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, circle_vbo_);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
    glBindVertexArray(0);
}

void Renderer::draw(double now, float aspect) {
    float time = static_cast<float>(now - start_time_);
    auto  vp   = camera_.view();
    auto  proj  = camera_.projection(aspect);

    // Terrain
    terrain_shader_->use();
    terrain_shader_->set_mat4("view",       vp);
    terrain_shader_->set_mat4("projection", proj);
    terrain_shader_->set_mat4("model",      glm::mat4(1.f));
    terrain_shader_->set_float("time",      time);
    draw_terrain();

    // Entities
    auto entities = em_.snapshot();
    draw_entities(entities, now);

    // Threat radii for hostiles
    for (const auto& e : entities)
        if (e.iff == IFF::Hostile) draw_threat_radius(e, now);
}

void Renderer::draw_terrain() {
    terrain_->draw();
}

void Renderer::draw_entities(const std::vector<Entity>& entities, double now) {
    auto vp   = camera_.view();
    auto proj  = camera_.projection(1.f); // recomputed per frame via draw()

    int w, h; glfwGetFramebufferSize(window_, &w, &h);
    float aspect = static_cast<float>(w) / static_cast<float>(h);
    proj = camera_.projection(aspect);

    entity_shader_->use();
    entity_shader_->set_mat4("view",       vp);
    entity_shader_->set_mat4("projection", proj);

    glBindVertexArray(billboard_vao_);

    for (const auto& e : entities) {
        glm::vec3 pos = em_.interpolated_position(e, now);

        // Scale: aircraft larger, ground vehicles smaller
        float size = (e.type == EntityType::Aircraft) ? 60.f : 30.f;

        glm::mat4 model = glm::translate(glm::mat4(1.f), pos);
        model = glm::scale(model, glm::vec3(size));

        glm::vec3 color = (e.iff == IFF::Friendly) ? COLOR_FRIENDLY
                        : (e.iff == IFF::Hostile)  ? COLOR_HOSTILE
                                                   : COLOR_UNKNOWN;

        entity_shader_->set_mat4("model", model);
        entity_shader_->set_vec3("color", color);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glBindVertexArray(0);
}

void Renderer::draw_threat_radius(const Entity& e, double now) {
    int w, h; glfwGetFramebufferSize(window_, &w, &h);
    float aspect = static_cast<float>(w) / static_cast<float>(h);

    glm::vec3 pos = em_.interpolated_position(e, now);
    float radius  = 200.f; // 200m engagement radius

    glm::mat4 model = glm::translate(glm::mat4(1.f), pos);
    model = glm::scale(model, glm::vec3(radius, 1.f, radius));

    float time = static_cast<float>(now - start_time_);

    threat_shader_->use();
    threat_shader_->set_mat4("view",       camera_.view());
    threat_shader_->set_mat4("projection", camera_.projection(aspect));
    threat_shader_->set_mat4("model",      model);
    threat_shader_->set_float("time",      time);

    glLineWidth(2.f);
    glBindVertexArray(circle_vao_);
    glDrawArrays(GL_LINE_LOOP, 0, 64);
    glBindVertexArray(0);
}
