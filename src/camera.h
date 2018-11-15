#pragma once

#include "shader.h"

#include <glm/glm.hpp>

class Camera
{
public:
    int width;
    int height;
    float fov;
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 direction = glm::vec3(0, 0, 1);
    glm::vec3 up = glm::vec3(0, 1, 0);

    /**
     * Create a camera of the given width, height and field of view.
     */
    Camera(int width, int height, int fov);

    /**
     * Configure the camera matrices for rendering the scene with the given shader.
     */
    void configure(Shader* shader);
};