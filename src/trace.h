#pragma once

#include "shader.h"
#include "camera.h"
#include "rigidbody.h"

#include <vector>

class Trace
{
public:
    const static float WIDTH; // trace line width
    const static float DELTA; // minimum distance between series points

    Trace(Shader* shader, Camera* camera, RigidBody* target);
    ~Trace();

    /**
     * Update the trace points by fetching the target's new coordinates.
     */
    void update();
    
    /**
     * Clear the previous series of points.
     */
    void reset();

    /**
     * Draw the trace in the scene.
     */
    void draw();

private:
    Shader* shader;
    Camera* camera; // scene camera that renders the trace
    RigidBody* target; // object to track and for which to trace the trajectory

    unsigned int VAO, VBO;
    std::vector<glm::vec3> points;

    /**
     * Draw a point series on the screen.
     */
    void drawSeries(std::vector<glm::vec3> &series);
};