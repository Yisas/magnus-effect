#pragma once

#include "transform.h"

#include <vector>

/**
 * Rigid body object.
 * 
 * Units:
 * Linear velocity -> m/s
 * Angular velocity -> rps
 */
class Rigidbody : public Transform
{
public:
    /**
     * Create a new rigid body for the given model.
     */
    Rigidbody(Model* model, float mass, float bounciness);

    // attribute getters and setters
    glm::vec3 getLinearVelocity() const;
    glm::vec3 getAngularVelocity() const;
    void setLinearVelocity(glm::vec3 newLinearVelocity);
    void setAngularVelocity(glm::vec3 newAngularVelocity);
    void setPosition(glm::vec3 newPosition) override;
    void setRotation(glm::quat newRotation) override;

    /**
     * Reset the rigid body to its initial positions and velocities.
     */
    void reset();

    /**
     * Update the rigid body attributes for the given time interval.
     */
    void update(float deltaTime);

private:
    const static float GROUND_COORDINATE;
    const static glm::vec3 GROUND_NORMAL;
    const static glm::vec3 GRAVITY;
    
    float mass;
    float bounciness;

    // initial conditions    
    glm::vec3 initialPosition = glm::vec3(0, 0, 0);
    glm::quat initialRotation = glm::mat3(1);
    glm::vec3 initialLinearVelocity = glm::vec3(0, 0, 0);
    glm::vec3 initialAngularVelocity = glm::vec3(0, 0, 0);

    // current state
    // the current position and rotation are part of the parent class
    glm::vec3 linearVelocity = glm::vec3(0, 0, 0);
    glm::vec3 angularVelocity = glm::vec3(0, 0, 0);

    /**
     * Check if the rigid body collides with the ground.
     */
    void checkCollision();
};