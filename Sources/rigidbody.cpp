#include "rigidbody.h"

#include <glm/gtc/quaternion.hpp>

const float Rigidbody::GROUND_COORDINATE = 0;
const glm::vec3 Rigidbody::GROUND_NORMAL = glm::vec3(0, 1, 0);
const glm::vec3 Rigidbody::GRAVITY = glm::vec3(0.0f, -9.81f, 0.0f);

Rigidbody::Rigidbody(Model* model, float mass, float bounciness)
    : Transform(model), mass(mass), bounciness(bounciness)
{

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

void Rigidbody::reset()
{
    position = initialPosition;
    rotation = initialRotation;
    linearVelocity = initialLinearVelocity;
    angularVelocity = initialAngularVelocity;
}

void Rigidbody::update(float deltaTime)
{
    glm::vec3 force = mass * GRAVITY;
    linearVelocity += force / mass * deltaTime;
    position += linearVelocity * deltaTime;
    rotation *= glm::quat(angularVelocity * (float)(2 * M_PI) * deltaTime);
    checkCollision();
}

void Rigidbody::checkCollision()
{
    if (position.y - scale.y <= GROUND_COORDINATE)
    {
        position.y = GROUND_COORDINATE + scale.y;
        linearVelocity = glm::reflect(linearVelocity, GROUND_NORMAL) * bounciness;
        angularVelocity *= bounciness;
    }
}