#include "camera.hpp"

Camera::Camera(int width, int height, int fov)
    : width(width), height(height), fov(fov)
{
    ratio = (float)width / (float)height;
}

void Camera::setPosition(glm::vec3 newPosition)
{
    position = newPosition;
}

void Camera::setDirection(glm::vec3 newDirection)
{
    direction = newDirection;
}

void Camera::configure(Shader* shader)
{
    shader->setVec3("viewPosition", position);
    shader->setMat4("projection", glm::perspective(glm::radians(fov), ratio, 0.1f, 100.0f));
    shader->setMat4("view", glm::lookAt(position, position + direction, up));
    shader->setMat4("model", glm::mat4(1));
}