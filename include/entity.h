#pragma once
#include <glm/glm.hpp>
#include <string>
#include <cstdint>

enum class IFF : uint8_t {
    Friendly = 0,
    Hostile  = 1,
    Unknown  = 2
};

enum class EntityType : uint8_t {
    Aircraft = 0,
    GroundVehicle = 1,
    Waypoint = 2
};

struct Entity {
    uint32_t   id;
    std::string callsign;
    EntityType  type;
    IFF         iff;

    glm::vec3 position;      // x,y = map coords (meters from origin), z = altitude (meters)
    glm::vec3 prev_position; // for dead-reckoning interpolation
    float     heading;       // degrees, 0=north
    float     speed;         // m/s

    double    last_update;   // seconds since epoch
    bool      active;
};
