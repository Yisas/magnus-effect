#pragma once

#include "transform.hpp"

#include <vector>

class Rigidbody : public Transform
{
public:
    /**
     * Create a new rigid body for the given model.
     */
    Rigidbody(Model* model, float mass);

private:
    float mass;
    std::vector<glm::vec3> forces;
    glm::vec3 velocity = glm::vec3(0, 0, 0);
    glm::vec3 angularVelocity = glm::vec3(0, 0, 0);
};