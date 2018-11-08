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

    // attribute setters
    void setLinearVelocity(glm::vec3 newLinearVelocity);

    /**
     * Subject the rigid body to gravitational force.
     */
    void addGravity();

    /**
     * Update the rigid body attributes for the given time interval.
     */
    void update(float deltaTime);

private:
    const static glm::vec3 GRAVITY;

    float mass;
    std::vector<glm::vec3> constantForces;
    glm::vec3 linearVelocity = glm::vec3(0, 0, 0);
    glm::vec3 angularVelocity = glm::vec3(0, 0, 0);
};