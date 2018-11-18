#include "trace.h"

using namespace std;

const float Trace::WIDTH = 1.5f;
const float Trace::DELTA = 0.01f;
const glm::vec3 Trace::COLOR = glm::vec3(1.0f, 0.5f, 0.2f);
const glm::vec3 Trace::PREVIOUS_COLOR = glm::vec3(0.75f, 0.3f, 0.1f);
bool Trace::keepPrevious;

Trace::Trace(RigidBody &target, Camera &camera, shared_ptr<Shader> shader)
    : target(target), camera(camera), shader(shader)
{

}

void Trace::update()
{
    if (points.size() <= 1 || glm::distance(target.position, points[points.size() - 1]) >= DELTA)
    {
        points.push_back(target.position);
    } 
}

void Trace::reset()
{
    if (points.size() == 0)
        return;
    if (keepPrevious)
        previousPoints = points;
    else
        previousPoints.clear();
    points.clear();
}

void Trace::draw()
{
    shader->use();
    camera.configure(*shader);
    if (keepPrevious && previousPoints.size() != 0)
        drawSeries(previousPoints, PREVIOUS_COLOR);
    drawSeries(points, COLOR);
}

void Trace::drawSeries(vector<glm::vec3> &series, glm::vec3 color)
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
    shader->setVec3("color", color);
    glLineWidth(WIDTH);
    glDrawArrays(GL_LINE_STRIP, 0, series.size());
    glLineWidth(1);

    // free resources
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}