#pragma once

#include "shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Light
{
public:
    /**
     * Create a new light of the given color.
     */
    Light(glm::vec3 color);

    /**
     * Place the light at the new position.
     */
    void move(glm::vec3 newPos);

    /**
     * Set up the light uniforms for rendering the scene with the given shader.
     */
    void setUp(Shader shader);

private:
    glm::vec3 color;
    glm::vec3 pos = glm::vec3(0, 0, 0);
};