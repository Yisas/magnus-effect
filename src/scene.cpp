#include "scene.h"

#include <iostream>
using namespace std;

const unsigned int Scene::SHADOW_RESOLUTION = 2048;

Scene::Scene(Shader* shader, Shader* depthShader, Camera* camera, Light* light)
    :shader(shader), depthShader(depthShader), camera(camera), light(light)
{
    glGenFramebuffers(1, &depthMapFBO);
    
    // create the depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_RESOLUTION, SHADOW_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    // attach the depth texture as an FBO depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene::~Scene()
{
    delete shader;
    delete depthShader;
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
    // render depth map for shadow mapping
    glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    depthShader->use();
    camera->configure(depthShader);
    light->configure(depthShader);
    drawObjects(depthShader);
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render scene using shadow map
    glViewport(0, 0, camera->width, camera->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader->use();
    camera->configure(shader);
    light->configure(shader); 
    shader->setInt("shadowMap", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    drawObjects(shader);
}

void Scene::drawObjects(Shader* shader)
{
    for (Transform* object : staticObjects)
    {
        object->draw(shader);
    }
    for (RigidBody* object : dynamicObjects)
    {
        object->draw(shader);
    }
}