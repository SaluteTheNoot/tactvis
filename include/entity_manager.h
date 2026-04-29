#pragma once
#include "entity.h"
#include <unordered_map>
#include <mutex>
#include <vector>

class EntityManager {
public:
    void upsert(const Entity& e);
    void remove(uint32_t id);

    // Returns a snapshot — safe to iterate on render thread
    std::vector<Entity> snapshot() const;

    // Interpolates position based on elapsed time since last packet
    glm::vec3 interpolated_position(const Entity& e, double now) const;

private:
    mutable std::mutex          mutex_;
    std::unordered_map<uint32_t, Entity> entities_;
};
