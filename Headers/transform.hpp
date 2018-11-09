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
    glm::vec3 getPosition() const;
    glm::mat3 getRotation() const;
    glm::vec3 getScale() const;
    virtual void setPosition(glm::vec3 newPosition);
    virtual void setRotation(glm::mat3 newRotation);
    virtual void setScale(glm::vec3 newScale);

    /**
     * Draw the transform model using the given shader.
     */
    void draw(Shader* shader);

protected:
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::mat3 rotation = glm::mat3(1);
    glm::vec3 scale = glm::vec3(1, 1, 1);

private:
    Model* model;
};