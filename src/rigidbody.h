#pragma once

#include "transform.h"

#include <vector>

class RigidBody : public Transform
{
public:
    static bool useGravity;
    static bool useMagnusForce;

    // properties
    float mass;
    float drag;
    float bounciness;

    // initial conditions
    glm::vec3 initialPosition = glm::vec3(0, 0, 0);
    glm::quat initialRotation = glm::mat3(1);
    glm::vec3 initialLinearVelocity = glm::vec3(0, 0, 0);
    glm::vec3 initialAngularVelocity = glm::vec3(0, 0, 0);

    /**
     * Create a new rigid body for the given model.
     */
    RigidBody(shared_ptr<Model> model, float mass, float drag, float bounciness);

    /**
     * Reset the rigid body to its initial positions and velocities.
     * This also pre-calculates its inertia tensor.
     */
    void initialize();

    /**
     * Update the rigid body attributes for the given time interval.
     */
    void update(float deltaTime);

private:
    const static float GROUND_COORDINATE;
    const static glm::vec3 GROUND_NORMAL;
    const static glm::vec3 GRAVITY;
    
    glm::vec3 centerOfMass;
    glm::mat3 bodySpaceInertiaTensor;
    glm::mat3 bodySpaceInertiaTensorInverse;

    // current state
    // the current position and rotation are part of transform, the parent class
    glm::vec3 linearVelocity = glm::vec3(0, 0, 0);
    glm::vec3 angularVelocity = glm::vec3(0, 0, 0);
    glm::vec3 angularMomentum = glm::vec3(0, 0, 0);
    std::vector<std::pair<glm::vec3, glm::vec3>> forces; // pairs of force and force position

    /**
     * Add a force to the rigid body.
     */
    void addForce(glm::vec3 force, glm::vec3 position);

    /**
     * Calculate the resulting force of all the forces applied on the rigid body.
     */
    glm::vec3 calculateResultingForce();

    /**
     * Calculate the torque generated by the forces applied on the rigid body.
     */
    glm::vec3 calculateTorque();

    /**
     * Check if the rigid body collides with the ground.
     */
    void checkCollision();
};