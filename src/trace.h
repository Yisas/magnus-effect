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
    const static glm::vec3 COLOR; // color of the trace
    const static glm::vec3 PREVIOUS_COLOR; // color of the previous trace series
    static bool keepPrevious; // also draw the previous trace series

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
    std::vector<glm::vec3> previousPoints;

    /**
     * Draw a point series on the screen.
     */
    void drawSeries(std::vector<glm::vec3> &series, glm::vec3 color);
};