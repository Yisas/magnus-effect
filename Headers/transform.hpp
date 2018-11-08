#pragma once

#include "shader.hpp"
#include "model.hpp"

#include <glm/glm.hpp>

class Transform
{
public:
    /**
     * Create a new transform for the given model.
     */
    Transform(Model* model);

    /**
     * Destructor.
     */
    virtual ~Transform();

    // attribute getters and setters
    glm::vec3 getPosition();
    glm::mat4 getRotation();
    glm::vec3 getSize();
    void setPosition(glm::vec3 newPosition);
    void setRotation(glm::mat4 newRotation);
    void setSize(glm::vec3 newSize);

    /**
     * Draw the transform model using the given shader.
     */
    void draw(Shader* shader);

private:
    Model* model;
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::mat4 rotation = glm::mat4(1);
    glm::vec3 size = glm::vec3(1, 1, 1);
};