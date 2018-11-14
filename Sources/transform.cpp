#include "transform.h"

Transform::Transform(Model* model)
    : model(model)
{

}

Transform::~Transform()
{
    delete model;
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