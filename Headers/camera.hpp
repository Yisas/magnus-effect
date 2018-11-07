#pragma once

#include "shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
    /**
     * Create a camera of the given width, height and field of view.
     */
    Camera(int width, int height, int fov);
    
    /**
     * Place the camera a the new position.
     */
    void move(glm::vec3 newPos);

    /**
     * Set up the camera matrices for rendering the scene with the given shader.
     */
    void setUp(Shader shader);

private:
    int width;
    int height;
    float fov;
    glm::vec3 pos = glm::vec3(0, 0, 0);
    glm::vec3 dir = glm::vec3(0, 0, 1);
    glm::vec3 up = glm::vec3(0, 1, 0);
};