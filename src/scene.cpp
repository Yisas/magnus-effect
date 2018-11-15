#include "scene.h"

Scene::Scene(Shader* shader, Camera* camera, Light* light)
    :shader(shader), camera(camera), light(light)
{

}

Scene::~Scene()
{
    delete shader;
    delete camera;
    delete light;
    for (Transform* object : staticObjects)
    {
        delete object;
    }
    staticObjects.clear();
    for (RigidBody* object : dynamicObjects)
    {
        delete object;
    }
    dynamicObjects.clear();
}

void Scene::reset()
{
    for (RigidBody* object : dynamicObjects)
    {
        object->reset();
    }
}

void Scene::update(float deltaTime)
{
    for (RigidBody* object : dynamicObjects)
    {
        object->update(deltaTime);
    }
}

void Scene::draw()
{
    shader->use();
    camera->configure(shader);
    light->configure(shader);        
    for (Transform* object : staticObjects)
    {
        object->draw(shader);
    }
    for (RigidBody* object : dynamicObjects)
    {
        object->draw(shader);
    }
}