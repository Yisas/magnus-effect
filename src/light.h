#pragma once

#include "shader.h"

#include <glm/glm.hpp>

class Light
{
public:
    glm::vec3 color;
    glm::vec3 position = glm::vec3(0, 0, 0);

    /**
     * Create a new light of the given color.
     */
    Light(glm::vec3 color);

    /**
     * Configure the light uniforms for rendering the scene with the given shader.
     */
    void configure(Shader* shader);
};