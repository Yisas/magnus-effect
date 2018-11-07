#include "light.hpp"

using namespace glm;

Light::Light(vec3 color)
    : color(color)
{

}

void Light::move(vec3 newPos)
{
    pos = newPos;
}

void Light::setUp(Shader shader)
{
    shader.setVec3("lightColor", color);
    shader.setVec3("lightPos", pos);
}

