#include "transform.hpp"

using namespace std;
using namespace glm;

Transform::Transform(Model* model)
    : model(model)
{

}

Transform::~Transform()
{
    delete model;
}

void Transform::setPosition(vec3 newPosition)
{
    position = newPosition;
}

void Transform::setRotation(mat4 newRotation)
{
    rotation = newRotation;
}

void Transform::setSize(vec3 newSize)
{
    size = newSize;
}

void Transform::draw(Shader* shader)
{
    mat4 transformation = translate(mat4(1), position)
        * rotation
        * scale(mat4(1), size);
    shader->setMat4("model", transformation);
    model->draw(*shader);
}