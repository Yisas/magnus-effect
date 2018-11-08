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

vec3 Transform::getPosition()
{
    return position;
}

mat4 Transform::getRotation()
{
    return rotation;
}

vec3 Transform::getSize()
{
    return size;
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