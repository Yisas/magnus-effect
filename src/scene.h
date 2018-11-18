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
    Camera camera;
    Light light;
    std::vector<Transform> staticObjects;
    std::vector<RigidBody> dynamicObjects;

    /**
     * Create a new empty scene.
     * The scene will be rendered by the given shaders, camera and light.
     */
    Scene(Camera camera, Light light, shared_ptr<Shader> shader, shared_ptr<Shader> depthShader);

    /**
     * Update the scene's rigid bodies to reflect the given delta time changes.
     */
    void update(float deltaTime);

    /**
     * Reset the positions and velocites of the rigid bodies in the scene.
     */
    void initialize();

    /**
     * Draw the scene.
     */
    void draw();

private:
    const static unsigned int SHADOW_RESOLUTION;
    unsigned int depthMap, depthMapFBO;
    shared_ptr<Shader> shader;
    shared_ptr<Shader> depthShader;

    /**
     * Draw the scene objects with the given shader.
     */
    void drawObjects(Shader &shader);
};