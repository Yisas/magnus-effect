#include "camera.h"

Camera::Camera(int width, int height, int fov)
    : width(width), height(height), fov(fov)
{
    
}

void Camera::configure(Shader* shader)
{
    float ratio = (float)width / (float)height;
    shader->setMat4("projection", glm::perspective(glm::radians(fov), ratio, 0.1f, 100.0f));
    shader->setMat4("view", glm::lookAt(position, position + direction, up));
    shader->setMat4("model", glm::mat4(1));
    shader->setVec3("viewPosition", position);
}