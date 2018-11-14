#include "shader.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "transform.h"
#include "rigidbody.h"
#include "scene.h"

#include <nanogui/nanogui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>

using namespace nanogui;
using namespace std;

const string TITLE = "Magnus Effect";
int width = 1280, height = 800;
float lastFrame, deltaTime;
float speed = 1;

GLFWwindow* window;
Screen* screen;
Scene* scene;

/**
 * Key callback event handler.
 */
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        scene->reset();
}

/**
 * Initialize OpenGL and create the application window.
 */
void createWindow()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    // glfw window creation
    window = glfwCreateWindow(width, height, TITLE.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
}

/**
 * Create the application GUI.
 */
void createGUI()
{

}

/**
 * Create the application scene.
 */
void createScene()
{
    Shader* shader = new Shader("Shaders/scene.vert", "Shaders/scene.frag");
    Camera* camera = new Camera(1200, 800, 45);
    camera->setPosition(glm::vec3(0, 1, -5));
    camera->setDirection(glm::vec3(0, 0, 1));
    Light* light = new Light(glm::vec3(1, 1, 1));
    light->setPosition(glm::vec3(0, 10, 0));
    scene = new Scene(shader, camera, light);
    
    Transform* plane = new Transform(new Model("Models/plane.blend"));
    plane->setRotation(glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(1, 0, 0)));
    plane->setScale(glm::vec3(2.74f, 1.0f, 0.76f));
    scene->staticObjects.push_back(plane);
    
    RigidBody* ball = new RigidBody(new Model("Models/ball.blend"), 0.0027f, 0.75f);
    ball->setScale(glm::vec3(0.04f));
    ball->setPosition(glm::vec3(-2, 1, 0));
    ball->setLinearVelocity(glm::vec3(3, 3, 0));
    ball->setAngularVelocity(glm::vec3(0, 0, -6.28f));
    scene->dynamicObjects.push_back(ball);
}

/**
 * Run the application main loop.
 */
void run()
{
    // rendering loop
    lastFrame = glfwGetTime();
    while (glfwWindowShouldClose(window) == false) {
        float currentFrame = glfwGetTime();
        deltaTime = (currentFrame - lastFrame) * speed;
        lastFrame = currentFrame;

        // background fill color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw scene
        scene->update(deltaTime);
        scene->draw();

        //screen->drawContents();
        //screen->drawWidgets();

        // flip buffers and draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}


/**
 * Create and run the Magnus Effect OpenGL application.
 */
int main()
{
    createWindow();
    createGUI();
    createScene();
    run();

    delete scene;
    delete screen;
    delete window;
    return EXIT_SUCCESS;
}