#include "light.h"

Light::Light(glm::vec3 color)
    : color(color)
{

}

void Light::configure(Shader* shader)
{
    shader->setVec3("lightColor", color);
    shader->setVec3("lightPosition", position);
}

