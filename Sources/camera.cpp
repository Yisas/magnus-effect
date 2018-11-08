#include "camera.hpp"

using namespace glm;

Camera::Camera(int width, int height, int fov)
    : width(width), height(height), fov(fov)
{

}

void Camera::setPosition(vec3 newPosition)
{
    position = newPosition;
}

void Camera::setDirection(vec3 newDirection)
{
    direction = newDirection;
}

void Camera::configure(Shader* shader)
{
    shader->setVec3("viewPosition", position);
    shader->setMat4("projection", perspective(radians(fov), (float)width / (float)height, 0.1f, 100.0f));
    shader->setMat4("view", lookAt(position, position + direction, up));
    shader->setMat4("model", mat4(1));
}