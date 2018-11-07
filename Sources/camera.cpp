#include "camera.hpp"

using namespace glm;

Camera::Camera(int width, int height, int fov)
    : width(width), height(height), fov(fov)
{

}

void Camera::move(vec3 newPos)
{
    pos = newPos;
}

void Camera::setUp(Shader shader)
{
    shader.setVec3("viewPos", pos);
    shader.setMat4("projection", perspective(radians(fov), (float)width / (float)height, 0.1f, 100.0f));
    shader.setMat4("view", lookAt(pos, pos + dir, up));
}