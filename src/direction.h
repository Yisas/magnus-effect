#pragma once

#include "shader.h"
#include "camera.h"
#include "rigidbody.h"

#include <vector>

class Direction
{
public:
    const static glm::vec3 LINEAR_VELOCITY_COLOR; // color of the linear velocity vector
    const static glm::vec3 ANGULAR_VELOCITY_COLOR; // color of the angular velocity vector

    Camera *camera; // scene camera that renders the direction
    RigidBody *target; // object for which to draw the direction
    
    /**
     * Create a new direction for the given target.
     * The direction will be rendered using the given shader and camera.
     */
    Direction(RigidBody *target, Camera *camera, shared_ptr<Shader> shader);

    /**
     * Draw the direction in the scene.
     */
    void draw();

private:
    shared_ptr<Shader> shader;

    /**
     * Draw a directional vector on the screen.
     */
    void drawVector(std::vector<glm::vec3> &points, glm::vec3 color);
};