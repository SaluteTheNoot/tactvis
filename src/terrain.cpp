#include "terrain.h"
#include <cmath>
#include <vector>

// Simple value noise for terrain height variation
static float noise(float x, float z) {
    int xi = static_cast<int>(std::floor(x));
    int zi = static_cast<int>(std::floor(z));
    auto rand2 = [](int a, int b) -> float {
        int n = a * 1619 + b * 31337;
        n = (n << 13) ^ n;
        return 1.f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.f;
    };
    float fx = x - xi, fz = z - zi;
    float u  = fx * fx * (3 - 2 * fx);
    float v  = fz * fz * (3 - 2 * fz);
    return glm::mix(
        glm::mix(rand2(xi,   zi),   rand2(xi+1, zi),   u),
        glm::mix(rand2(xi,   zi+1), rand2(xi+1, zi+1), u), v);
}

static float fbm(float x, float z) {
    float val = 0, amp = 1, freq = 1;
    for (int i = 0; i < 4; ++i) {
        val  += noise(x * freq, z * freq) * amp;
        amp  *= 0.5f;
        freq *= 2.f;
    }
    return val;
}

Terrain::Terrain(int grid_size, float world_scale)
    : grid_size_(grid_size), world_scale_(world_scale)
{
    generate();
}

Terrain::~Terrain() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

void Terrain::generate() {
    int N = grid_size_;
    heights_.resize((N+1) * (N+1));

    struct Vert { float x, y, z, nx, ny, nz; };
    std::vector<Vert> verts;
    verts.reserve((N+1) * (N+1));

    float scale = world_scale_ / N;
    float height_scale = world_scale_ * 0.02f;
    float noise_scale  = 1.f / (N * 0.3f);

    for (int z = 0; z <= N; ++z) {
        for (int x = 0; x <= N; ++x) {
            float wx = (x - N/2) * scale;
            float wz = (z - N/2) * scale;
            float h  = fbm(x * noise_scale, z * noise_scale) * height_scale;
            heights_[z * (N+1) + x] = h;
            verts.push_back({wx, h, wz, 0, 1, 0});
        }
    }

    // Compute normals
    for (int z = 1; z < N; ++z) {
        for (int x = 1; x < N; ++x) {
            float hL = heights_[z*(N+1)+(x-1)];
            float hR = heights_[z*(N+1)+(x+1)];
            float hD = heights_[(z+1)*(N+1)+x];
            float hU = heights_[(z-1)*(N+1)+x];
            glm::vec3 n = glm::normalize(glm::vec3(hL-hR, 2.f*scale, hD-hU));
            auto& v = verts[z*(N+1)+x];
            v.nx = n.x; v.ny = n.y; v.nz = n.z;
        }
    }

    std::vector<unsigned int> indices;
    indices.reserve(N * N * 6);
    for (int z = 0; z < N; ++z) {
        for (int x = 0; x < N; ++x) {
            unsigned int tl = z*(N+1)+x, tr = tl+1;
            unsigned int bl = (z+1)*(N+1)+x, br = bl+1;
            indices.insert(indices.end(), {tl, bl, tr, tr, bl, br});
        }
    }
    index_count_ = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vert), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)(3*sizeof(float)));
    glBindVertexArray(0);
}

void Terrain::draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

float Terrain::height_at(float x, float z) const {
    float half = world_scale_ * 0.5f;
    float step = world_scale_ / grid_size_;
    int xi = static_cast<int>((x + half) / step);
    int zi = static_cast<int>((z + half) / step);
    xi = std::clamp(xi, 0, grid_size_);
    zi = std::clamp(zi, 0, grid_size_);
    return heights_[zi * (grid_size_+1) + xi];
}
