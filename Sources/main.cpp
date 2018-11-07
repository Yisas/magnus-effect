#include "glitter.hpp"
#include "shader.hpp"
#include "model.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>

GLFWwindow* initialize() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    
    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    return window;
}

int main() {
    // initialize opengl
    GLFWwindow* window = initialize();
    if (window == nullptr) {
        return EXIT_FAILURE;
    }

    Shader shader("Shaders/standard.vert", "Shaders/standard.frag");
    Model model("Models/ball.blend");

    // Rendering Loop
    while (glfwWindowShouldClose(window) == false) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw Scene
        shader.use();
        shader.setMat4("projection", glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f));
        shader.setMat4("view", glm::lookAt(glm::vec3(0, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
        shader.setMat4("model", glm::mat4(1));
        model.Draw(shader);

        // Flip Buffers and Draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return EXIT_SUCCESS;
}
