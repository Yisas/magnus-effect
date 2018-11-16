#include "light.h"

void Light::configure(Shader* shader)
{
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 100.0f);
    glm::mat4 lightView = glm::lookAt(position, position + direction, up);
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    shader->setVec3("lightPosition", position);
    shader->setVec3("lightColor", color);
}

