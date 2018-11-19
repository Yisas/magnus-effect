#include "transform.h"

Transform::Transform(shared_ptr<Model> model)
    : model(model)
{

}

void Transform::draw(Shader &shader)
{
    glm::mat4 transformation = glm::mat4(1);
    transformation = glm::translate(transformation, position);
    transformation *= glm::mat4(rotation);
    transformation = glm::scale(transformation, scale / 2.0f);
    shader.setMat4("model", transformation);
    model->draw(shader);
}