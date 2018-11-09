#include "rigidbody.hpp"

#include <glm/gtc/quaternion.hpp>

const glm::vec3 Rigidbody::GRAVITY = glm::vec3(0.0f, -9.81f, 0.0f);
const float Rigidbody::GROUND_COORDINATE = 0;

Rigidbody::Rigidbody(Model* model, float mass)
    : Transform(model), mass(mass)
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

void Rigidbody::setRotation(glm::mat3 newRotation)
{
    Transform::setRotation(newRotation);
    initialRotation = newRotation;
}

void Rigidbody::setScale(glm::vec3 newScale)
{
    Transform::setScale(newScale);
    initialScale = newScale;
}

void Rigidbody::reset()
{
    enabled = true;
    position = initialPosition;
    rotation = initialRotation;
    scale = initialScale;
    linearVelocity = initialLinearVelocity;
    angularVelocity = initialAngularVelocity;
}

void Rigidbody::update(float deltaTime)
{
    if (enabled)
    {
        glm::vec3 force = mass * GRAVITY;
        linearVelocity += force / mass * deltaTime;
        position += linearVelocity * deltaTime;
        rotation = glm::mat3(glm::quat(rotation) * glm::quat(angularVelocity * deltaTime));
        checkCollision();
    }
}

void Rigidbody::checkCollision()
{
    if (position.y - scale.y <= GROUND_COORDINATE)
    {
        position.y = GROUND_COORDINATE + scale.y;
        enabled = false;
    }
}