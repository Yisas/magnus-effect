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
    const static unsigned int SHADOW_RESOLUTION;

    Shader* shader;
    Shader* depthShader;
    Camera* camera;
    Light* light;

    std::vector<Transform*> staticObjects;
    std::vector<RigidBody*> dynamicObjects;

    Scene(Shader* shader, Shader* depthShader, Camera* camera, Light* light);
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
    unsigned int depthMap, depthMapFBO;

    /**
     * Draw the scene objects with the given shader.
     */
    void drawObjects(Shader* shader);
};