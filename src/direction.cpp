#include "direction.h"

#include <iostream>
using namespace std;

const glm::vec3 Direction::LINEAR_VELOCITY_COLOR = glm::vec3(0.1f, 0.6f, 0.1f);
const glm::vec3 Direction::ANGULAR_VELOCITY_COLOR = glm::vec3(1.0f, 0.8f, 0.2f);

Direction::Direction(RigidBody *target, Camera *camera, shared_ptr<Shader> shader)
    : target(target), camera(camera), shader(shader)
{

}

void Direction::draw()
{
    shader->use();
    camera->configure(*shader);

    glm::vec3 towards;

    vector<glm::vec3> linearVelocity = vector<glm::vec3>();
    linearVelocity.push_back(target->position);
    towards = target->linearVelocity * (1 + target->scale.x / 2.0f) / 10.0f;
    linearVelocity.push_back(target->position + towards);
    drawVector(linearVelocity, LINEAR_VELOCITY_COLOR);
    
    vector<glm::vec3> angularVelocity = vector<glm::vec3>();
    angularVelocity.push_back(target->position);
    towards = target->angularVelocity * (1 + target->scale.x / 2.0f) / 100.0f;
    angularVelocity.push_back(target->position + towards);
    drawVector(angularVelocity, ANGULAR_VELOCITY_COLOR);
}

void Direction::drawVector(vector<glm::vec3> &points, glm::vec3 color)
{
    // create buffers
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // draw series
    shader->setVec3("color", color);
    glDrawArrays(GL_LINE_STRIP, 0, points.size());

    // free resources
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}