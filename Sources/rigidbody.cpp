#include "rigidbody.h"

#include <glm/gtc/quaternion.hpp>

#include <iostream>

using namespace std;

const float Rigidbody::GROUND_COORDINATE = 0;
const glm::vec3 Rigidbody::GROUND_NORMAL = glm::vec3(0, 1, 0);
const glm::vec3 Rigidbody::GRAVITY = glm::vec3(0.0f, -9.81f, 0.0f);

Rigidbody::Rigidbody(Model* model, float mass, float bounciness)
    : Transform(model), mass(mass), bounciness(bounciness)
{
    centerOfMass = glm::vec3(0, 0, 0);
    bodySpaceInertiaTensor = glm::mat3(1); // TODO: replace with actual sphere inertia tensor
    bodySpaceInertiaTensorInverse = glm::inverse(bodySpaceInertiaTensor);
}

glm::vec3 Rigidbody::getLinearVelocity() const
{
    return linearVelocity;
}

glm::vec3 Rigidbody::getAngularVelocity() const
{
    return angularVelocity;
}

void Rigidbody::setLinearVelocity(glm::vec3 newLinearVelocity)
{
    initialLinearVelocity = newLinearVelocity;
    linearVelocity = newLinearVelocity;
}

void Rigidbody::setAngularVelocity(glm::vec3 newAngularVelocity)
{
    initialAngularVelocity = newAngularVelocity;
    angularVelocity = newAngularVelocity;
    angularMomentum = bodySpaceInertiaTensor * angularVelocity;
}

void Rigidbody::setPosition(glm::vec3 newPosition)
{
    Transform::setPosition(newPosition);
    initialPosition = newPosition;
}

void Rigidbody::setRotation(glm::quat newRotation)
{
    Transform::setRotation(newRotation);
    initialRotation = newRotation;
}

void Rigidbody::setMass(float newMass)
{
    mass = newMass;
}

void Rigidbody::setBounciness(float newBounciness)
{
    bounciness = newBounciness;
}

void Rigidbody::reset()
{
    position = initialPosition;
    rotation = initialRotation;
    linearVelocity = initialLinearVelocity;
    angularVelocity = initialAngularVelocity;
    angularMomentum = bodySpaceInertiaTensor * angularVelocity;
}

void Rigidbody::update(float deltaTime)
{
    // forces
    addForce(mass * GRAVITY, centerOfMass);
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

void Rigidbody::addForce(glm::vec3 force, glm::vec3 position)
{
    forces.push_back(pair<glm::vec3, glm::vec3>(force, position));
}

glm::vec3 Rigidbody::calculateResultingForce()
{
    glm::vec3 resultingForce = glm::vec3(0);
    for (pair<glm::vec3, glm::vec3> force : forces)
    {
        resultingForce += force.first;
    }
    return resultingForce;
}

glm::vec3 Rigidbody::calculateTorque()
{
    glm::vec3 torque = glm::vec3(0);
    for (pair<glm::vec3, glm::vec3> force : forces)
    {
        torque += cross(force.second - centerOfMass, force.first);
    }
    return torque;
}

void Rigidbody::checkCollision()
{
    if (position.y - scale.y <= GROUND_COORDINATE)
    {
        position.y = GROUND_COORDINATE + scale.y;
        linearVelocity = glm::reflect(linearVelocity, GROUND_NORMAL) * bounciness;
        angularMomentum *= bounciness;
    }
}