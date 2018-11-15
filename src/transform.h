#pragma once

#include "shader.h"
#include "model.h"

#include <glm/glm.hpp>

class Transform
{
public:
    // attributes
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation = glm::mat3(1);
    glm::vec3 scale = glm::vec3(1, 1, 1);

    /**
     * Create a new transform for the given model.
     */
    Transform(Model* model);

    /**
     * Destructor.
     */
    virtual ~Transform();

    /**
     * Draw the transform model using the given shader.
     */
    void draw(Shader* shader);

protected:
    Model* model;
};