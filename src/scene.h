#pragma once

#include "shader.h"
#include "camera.h"
#include "light.h"
#include "transform.h"
#include "rigidbody.h"

#include <vector>

class Scene
{
public:
    Shader* shader;
    Camera* camera;
    Light* light;

    std::vector<Transform*> staticObjects;
    std::vector<RigidBody*> dynamicObjects;

    Scene(Shader* shader, Camera* camera, Light* light);
    ~Scene();

    /**
     * Update the scene's rigid bodies to reflect the given delta time changes.
     */
    void update(float deltaTime);

    /**
     * Reset the positions and velocites of the rigid bodies in the scene.
     */
    void reset();

    /**
     * Draw the scene.
     */
    void draw();

private:

};