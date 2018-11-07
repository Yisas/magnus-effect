#include "shader.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace glm;

class MagnusEffect
{
public:
    /**
     * Create an run the Magnus Effect OpenGL application.
     */
    MagnusEffect(string title, int width, int height)
        : title(title), width(width), height(height)
    {
        initialize();
        
        camera = new Camera(width, height, 45);
        camera->move(vec3(0, 0, -10));
        light = new Light(vec3(1, 1, 1));
        light->move(vec3(0, 25, 0));
        shader = new Shader("Shaders/standard.vert", "Shaders/standard.frag");
        ball = new Model("Models/ball.blend");
        
        run();
    }

    /**
     * Destructor.
     */
    ~MagnusEffect()
    {
        delete camera;
        delete shader;
        delete ball;
        glfwTerminate();
    }

private:
    // window attributes
    GLFWwindow* window;
    string title;
    int width;
    int height;

    // rendering elements
    Shader* shader;
    Camera* camera;
    Light* light;
    Model* ball;

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
     * Run the application main loop.
     */
    void run()
    {
        // rendering loop
        while (glfwWindowShouldClose(window) == false) {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            // background fill color
            glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // draw scene
            draw(shader);

            // flip buffers and draw
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    /**
     * Draw the scene with the given shader.
     */
    void draw(Shader* shader)
    {
        shader->use();
        camera->setUp(*shader);
        light->setUp(*shader);
        shader->setMat4("model", mat4(1));
        ball->draw(*shader);
    }
};


/**
 * Create and run the Magnus Effect OpenGL application.
 */
int main()
{
    MagnusEffect("Magnus Effect", 1280, 800);
    return EXIT_SUCCESS;
}