#include "rigidbody.h"

#include <glm/gtc/quaternion.hpp>

#include <iostream>

using namespace std;

bool RigidBody::useGravity = true;
bool RigidBody::useMagnusForce = false;

const float RigidBody::GROUND_COORDINATE = 0;
const glm::vec3 RigidBody::GROUND_NORMAL = glm::vec3(0, 1, 0);
const glm::vec3 RigidBody::GRAVITY = glm::vec3(0.0f, -9.81f, 0.0f);

RigidBody::RigidBody(Model* model, float mass, float bounciness)
    : Transform(model), mass(mass), bounciness(bounciness)
{
    centerOfMass = glm::vec3(0, 0, 0);
    bodySpaceInertiaTensor = glm::mat3(1); // TODO: replace with actual sphere inertia tensor
    bodySpaceInertiaTensorInverse = glm::inverse(bodySpaceInertiaTensor);
}

void RigidBody::reset()
{
    position = initialPosition;
    rotation = initialRotation;
    linearVelocity = initialLinearVelocity;
    angularVelocity = initialAngularVelocity;
    angularMomentum = bodySpaceInertiaTensor * angularVelocity;
}

void RigidBody::update(float deltaTime)
{
    // forces
    if (useGravity)
        addForce(mass * GRAVITY, centerOfMass);
    if (useMagnusForce)
        addForce(glm::vec3(0), centerOfMass); // TODO: replace with actual formula
    glm::vec3 force = calculateResultingForce();
    
    // linear displacement
    linearVelocity += force / mass * deltaTime;
    position += linearVelocity * deltaTime;
    
    // angular displacement
    glm::vec3 torque = calculateTorque();
    angularMomentum += torque * deltaTime;
    glm::mat3 inertiaTensorInverse = glm::mat3(rotation)
        * bodySpaceInertiaTensorInverse
        * transpose(glm::mat3(rotation));
    angularVelocity = inertiaTensorInverse * angularMomentum;
    rotation *= glm::quat(angularVelocity * deltaTime);

    // clean up
    forces.clear();
    checkCollision();
}

void RigidBody::addForce(glm::vec3 force, glm::vec3 position)
{
    forces.push_back(pair<glm::vec3, glm::vec3>(force, position));
}

glm::vec3 RigidBody::calculateResultingForce()
{
    glm::vec3 resultingForce = glm::vec3(0);
    for (pair<glm::vec3, glm::vec3> force : forces)
    {
        resultingForce += force.first;
    }
    return resultingForce;
}

glm::vec3 RigidBody::calculateTorque()
{
    glm::vec3 torque = glm::vec3(0);
    for (pair<glm::vec3, glm::vec3> force : forces)
    {
        torque += cross(force.second - centerOfMass, force.first);
    }
    return torque;
}

void RigidBody::checkCollision()
{
    if (position.y - scale.y <= GROUND_COORDINATE)
    {
        position.y = GROUND_COORDINATE + scale.y;
        linearVelocity = glm::reflect(linearVelocity, GROUND_NORMAL) * bounciness;
        angularMomentum *= bounciness;
    }
}