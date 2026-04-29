#pragma once
#include <glad/glad.h>
#include <vector>

// Procedural heightmap terrain — a flat grid with subtle noise for visual interest.
class Terrain {
public:
    Terrain(int grid_size = 128, float world_scale = 4000.f);
    ~Terrain();

    void draw() const;
    float height_at(float x, float z) const;

    float world_scale() const { return world_scale_; }

private:
    void generate();

    int   grid_size_;
    float world_scale_;

    GLuint vao_{0}, vbo_{0}, ebo_{0};
    GLsizei index_count_{0};

    std::vector<float> heights_;
};
