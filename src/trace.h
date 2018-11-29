#pragma once

#include "shader.h"
#include "camera.h"
#include "rigidbody.h"

#include <vector>

class Trace
{
public:
    const static float DELTA; // minimum distance between series points
    const static glm::vec3 COLOR; // color of the trace
    const static glm::vec3 PREVIOUS_COLOR; // color of the previous trace series
    static bool keepPrevious; // also draw the previous trace series

    Camera *camera; // scene camera that renders the trace
    RigidBody *target; // object to track and for which to trace the trajectory

    /**
     * Create a new trace for the given target.
     * The trace will be rendered using the given shader and camera.
     */
    Trace(RigidBody *target, Camera *camera, shared_ptr<Shader> shader);

    /**
     * Update the trace points by fetching the target's new coordinates.
     */
    void update();
    
    /**
     * Clear the previous series of points.
     */
    void reset();

    /**
     * Clear both the current and previous series of points.
     */
    void clear();

    /**
     * Draw the trace in the scene.
     */
    void draw();

private:
    shared_ptr<Shader> shader;
    
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> previousPoints;

    /**
     * Draw a point series on the screen.
     */
    void drawSeries(std::vector<glm::vec3> &series, glm::vec3 color);
};