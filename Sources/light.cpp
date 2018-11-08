#include "light.hpp"

using namespace glm;

Light::Light(vec3 color)
    : color(color)
{

}

void Light::setPosition(vec3 newPosition)
{
    position = newPosition;
}

void Light::configure(Shader* shader)
{
    shader->setVec3("lightColor", color);
    shader->setVec3("lightPosition", position);
}

