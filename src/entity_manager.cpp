#include "entity_manager.h"
#include <glm/glm.hpp>
#include <cmath>

void EntityManager::upsert(const Entity& e) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = entities_.find(e.id);
    if (it != entities_.end()) {
        // Preserve previous position for dead-reckoning
        Entity updated = e;
        updated.prev_position = it->second.position;
        it->second = updated;
    } else {
        Entity fresh = e;
        fresh.prev_position = e.position;
        entities_[e.id] = fresh;
    }
}

void EntityManager::remove(uint32_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    entities_.erase(id);
}

std::vector<Entity> EntityManager::snapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Entity> out;
    out.reserve(entities_.size());
    for (const auto& [id, e] : entities_)
        if (e.active) out.push_back(e);
    return out;
}

glm::vec3 EntityManager::interpolated_position(const Entity& e, double now) const {
    // Dead-reckoning: project forward from last known position using heading+speed
    double dt = now - e.last_update;
    if (dt <= 0.0 || e.speed < 0.01f) return e.position;

    float rad = glm::radians(e.heading);
    glm::vec3 delta(
        std::sin(rad) * e.speed * static_cast<float>(dt),
        0.f,
        -std::cos(rad) * e.speed * static_cast<float>(dt)
    );
    return e.position + delta;
}
