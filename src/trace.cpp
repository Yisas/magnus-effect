#include "trace.h"

#include <iostream>
using namespace std;

const float Trace::WIDTH = 1.0f;
const float Trace::DELTA = 0.01f;

Trace::Trace(Shader* shader, Camera* camera, RigidBody* target)
    :shader(shader), camera(camera), target(target)
{

}

Trace::~Trace()
{
    delete shader;
}

void Trace::update()
{
    if (points.size() <= 1 || glm::distance(target->position, points[points.size() - 1]) >= DELTA)
    {
        points.push_back(target->position);
    }  
    cout << points.size() << endl;
}

void Trace::reset()
{
    points.clear();
}

void Trace::draw()
{
    shader->use();
    camera->configure(shader);
    drawSeries(points);
}

void Trace::drawSeries(vector<glm::vec3> &series)
{
    // create buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, series.size() * sizeof(glm::vec3), &series[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // draw series
    glLineWidth(WIDTH);
    glDrawArrays(GL_LINE_STRIP, 0, points.size());
    glLineWidth(1);

    // free resources
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}