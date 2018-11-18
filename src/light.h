#pragma once

#include "shader.h"

class Light
{
public:
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, -0.1f);
    glm::vec3 up = glm::vec3(0, 1, 0);

    /**
     * Configure the light uniforms for rendering the scene with the given shader.
     */
    void configure(Shader &shader);
};