#include "transform.h"

Transform::Transform(Model* model)
    : model(model)
{

}

Transform::~Transform()
{
    delete model;
}

glm::vec3 Transform::getScale() const
{
    return scale;
}

glm::vec3 Transform::getPosition() const
{
    return position;
}

glm::quat Transform::getRotation() const
{
    return rotation;
}

void Transform::setScale(glm::vec3 newScale)
{
    scale = newScale;
}

void Transform::setPosition(glm::vec3 newPosition)
{
    position = newPosition;
}

void Transform::setRotation(glm::quat newRotation)
{
    rotation = newRotation;
}

void Transform::draw(Shader* shader)
{
    glm::mat4 transformation = glm::mat4(1);
    transformation = glm::translate(transformation, position);
    transformation *= glm::mat4(rotation);
    transformation = glm::scale(transformation, scale);
    shader->setMat4("model", transformation);
    model->draw(*shader);
}