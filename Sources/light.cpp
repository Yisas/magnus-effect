#include "light.hpp"

Light::Light(glm::vec3 color)
    : color(color)
{

}

void Light::setPosition(glm::vec3 newPosition)
{
    position = newPosition;
}

void Light::configure(Shader* shader)
{
    shader->setVec3("lightColor", color);
    shader->setVec3("lightPosition", position);
}

