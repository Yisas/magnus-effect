#include "shader.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "transform.h"
#include "rigidbody.h"

#include <nanogui/nanogui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <iostream>

using namespace nanogui;
using namespace std;

class MagnusEffect
{
public:
    // Current instance of the Magnus Effect simulation
    // This is used to access the object from static methods, such as event handlers.
    static MagnusEffect* simulation;

    /**
     * Create and run the Magnus Effect OpenGL application.
     */
    MagnusEffect(string title, int width, int height)
        : title(title), width(width), height(height)
    {
        simulation = this;
        initialize();

        screen = new Screen();
        screen->initialize(window, true);
        FormHelper *gui = new FormHelper(screen);
        nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Controls");
        screen->setVisible(true);
        screen->performLayout();
        
        shader = new Shader("Shaders/scene.vert", "Shaders/scene.frag");
        
        camera = new Camera(width, height, 45);
        camera->setPosition(glm::vec3(0, 1, -5));
        camera->setDirection(glm::vec3(0, 0, 1));
        
        light = new Light(glm::vec3(1, 1, 1));
        light->setPosition(glm::vec3(0, 10, 0));
        
        plane = new Transform(new Model("Models/plane.blend"));
        plane->setRotation(glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(1, 0, 0)));
        plane->setScale(glm::vec3(2.74f, 1.0f, 0.76f));
        
        ball = new Rigidbody(new Model("Models/ball.blend"), 0.0027f, 0.75f);
        ball->setScale(glm::vec3(0.04f));
        ball->setPosition(glm::vec3(-2, 1, 0));
        ball->setLinearVelocity(glm::vec3(3, 3, 0));
        ball->setAngularVelocity(glm::vec3(0, 0, -6.28f));

        run();
    }

    /**
     * Destructor.
     */
    ~MagnusEffect()
    {
        delete screen;
        delete camera;
        delete shader;
        delete light;
        delete plane;
        delete ball;
        glfwTerminate();
    }

    /**
     * Reset the positions and velocites of the rigid bodies in the scene.
     */
    void reset()
    {
        cout << "Resetting simulation" << endl;
        ball->reset();
    }

private:
    // window attributes
    GLFWwindow* window;
    Screen* screen;
    string title;
    int width, height;
    float lastFrame, deltaTime;
    float speed = 1;

    // rendering elements
    Shader* shader;
    Camera* camera;
    Light* light;
    Transform* plane;
    Rigidbody* ball;

    /**
     * Initialize OpenGL and create the application window.
     */
    void initialize()
    {
        // glfw: initialize and configure
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        
        // glfw window creation
        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
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
     * Draw the scene with the given shader.
     */
    void draw(Shader* shader)
    {
        shader->use();
        camera->configure(shader);
        light->configure(shader);        
        plane->draw(shader);
        ball->update(deltaTime);
        ball->draw(shader);
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
            draw(shader);

            screen->drawContents();
            screen->drawWidgets();

            // flip buffers and draw
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    /**
     * Key callback event handler.
     */
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
            simulation->reset();
    }
};

MagnusEffect* MagnusEffect::simulation = nullptr;


/**
 * Create and run the Magnus Effect OpenGL application.
 */
int main()
{
    MagnusEffect("Magnus Effect", 1280, 800);
    return EXIT_SUCCESS;
}