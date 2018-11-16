#include "shader.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "transform.h"
#include "rigidbody.h"
#include "scene.h"
#include "trace.h"

#include <nanogui/nanogui.h>

#include <iostream>

using namespace nanogui;
using namespace std;

const string TITLE = "Magnus Effect";
int width = 1600, height = 900;
float lastFrame, deltaTime;
float playbackSpeed = 1;
bool playing = false;
bool traceTrajectory;

GLFWwindow* window;
Screen* screen;
FloatBox<float>* speedBox;
Slider* speedSlider;
Button* resetButton;
Button* playButton;
Scene* scene;
Trace* trace;

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
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    // glfw window creation
    window = glfwCreateWindow(width, height, TITLE.c_str(), nullptr, nullptr);
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
}

/**
 * Create a composite box widget to input a vector value.
 */
nanogui::ref<Widget> createVectorBox(Widget* parent, glm::vec3* vector)
{
    AdvancedGridLayout* parentLayout = (AdvancedGridLayout*)parent->layout();
    if (parentLayout->rowCount() > 0)
        parentLayout->appendRow(5);
    nanogui::ref<Widget> vectorBox = new Widget(parent);
    nanogui::ref<BoxLayout> layout = new BoxLayout(Orientation::Horizontal);
    layout->setSpacing(3);
    vectorBox->setLayout(layout);
    string axes[] = {"x", "y", "z"};
    for (string axis : axes)
    {
        nanogui::ref<Label> label = new Label(vectorBox, axis);
        nanogui::ref<FloatBox<float>> box = new FloatBox<float>(vectorBox);
        box->setEditable(true);
        box->setFixedSize(Vector2i(50, 20));
        box->setFontSize(16);
        box->setAlignment(TextBox::Alignment::Right);
        if (axis == "x")
        {
            box->setValue(vector->x);
            box->setCallback([=](float value) {
                vector->x = value;
                resetButton->callback()();
            });
        }
        else if (axis == "y")
        {
            box->setValue(vector->y);
            box->setCallback([=](float value) {
                vector->y = value;
                resetButton->callback()();
            });
        }
        else
        {
            box->setValue(vector->z);
            box->setCallback([=](float value) {
                vector->z = value;
                resetButton->callback()();
            });
        }
    }
    return vectorBox;
}

/**
 * Create the application GUI.
 */
void createGUI()
{
    // create GUI
    screen = new Screen();
    screen->initialize(window, true);
    FormHelper *gui = new FormHelper(screen);
    nanogui::ref<Window> optionsWindow = gui->addWindow(Eigen::Vector2i(20, 20), "Simulation");
    nanogui::ref<Label> settings = gui->addGroup("Ball settings");
    RigidBody* ball = scene->dynamicObjects[0];
    gui->addVariable<float>("Size",
        [=](float size) { ball->scale = glm::vec3(size); },
        [=]() { return ball->scale.x; }
    );
    gui->addVariable("Mass", ball->mass);
    gui->addVariable("Drag", ball->drag);
    gui->addVariable("Bounciness", ball->bounciness);
    gui->addWidget("Initial position", createVectorBox(optionsWindow, &ball->initialPosition));
    gui->addWidget("Linear velocity", createVectorBox(optionsWindow, &ball->initialLinearVelocity));
    gui->addWidget("Angular velocity", createVectorBox(optionsWindow, &ball->initialAngularVelocity));

    gui->addGroup("Forces");
    gui->addVariable("Gravitational force", RigidBody::useGravity);
    gui->addVariable("Magnus force", RigidBody::useMagnusForce);

    gui->addGroup("Rendering");
    gui->addVariable("Trace trajectory", traceTrajectory);
    gui->addVariable("Keep previous", Trace::keepPrevious);

    gui->addGroup("Controls");
    speedBox = gui->addVariable("Playback speed", playbackSpeed);
    speedBox->setCallback([=](float value) {
        playbackSpeed = value;
        speedSlider->setValue(value);
    });
    speedSlider = new Slider(optionsWindow);
    speedSlider->setRange(pair<float, float>(0, 2));
    speedSlider->setValue(playbackSpeed);
    speedSlider->setCallback([=](float value) {
        playbackSpeed = value;
        speedBox->setValue(value);
    });
    gui->addWidget(" ", speedSlider);
    resetButton = gui->addButton("Reset", []() {
        playing = false;
        playButton->setCaption("Play");
        scene->initialize();
        trace->reset();
    });
    playButton = gui->addButton("Play", []() {
        playing = !playing;
        playButton->setCaption(playing? "Stop" : "Play");
    });
    screen->setVisible(true);
    screen->performLayout();

    // event callbacks

    glfwSetCursorPosCallback(window,
            [](GLFWwindow *, double x, double y) {
            screen->cursorPosCallbackEvent(x, y);
        }
    );

    glfwSetMouseButtonCallback(window,
        [](GLFWwindow *, int button, int action, int modifiers) {
            screen->mouseButtonCallbackEvent(button, action, modifiers);
        }
    );

    glfwSetKeyCallback(window,
        [](GLFWwindow *, int key, int scancode, int action, int mods) {
            screen->keyCallbackEvent(key, scancode, action, mods);
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
            if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
            {
                if (playing)
                    resetButton->callback()();
                else
                    playButton->callback()();
            }
        }
    );

    glfwSetCharCallback(window,
        [](GLFWwindow *, unsigned int codepoint) {
            screen->charCallbackEvent(codepoint);
        }
    );

    glfwSetDropCallback(window,
        [](GLFWwindow *, int count, const char **filenames) {
            screen->dropCallbackEvent(count, filenames);
        }
    );

    glfwSetScrollCallback(window,
        [](GLFWwindow *, double x, double y) {
            screen->scrollCallbackEvent(x, y);
       }
    );

    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow *, int newWidth, int newHeight) {
            width = newWidth;
            height = newHeight;
            screen->resizeCallbackEvent(width, height);
            glViewport(0, 0, width, height);
            scene->camera->width = width;
            scene->camera->height = height;
        }
    );
}

/**
 * Create the application scene.
 */
void createScene()
{
    Shader* shader = new Shader("shaders/scene.vert", "shaders/scene.frag");
    Shader* depthShader = new Shader("shaders/depth.vert", "shaders/depth.frag");
    Camera* camera = new Camera(width, height, 45);
    camera->position = glm::vec3(0, 1, -5);
    Light* light = new Light();
    light->position = glm::vec3(0, 10, 0);
    scene = new Scene(shader, depthShader, camera, light);
    
    Transform* plane = new Transform(new Model("models/plane.blend"));
    plane->rotation = glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(1, 0, 0));
    plane->scale = glm::vec3(2.74f, 1.0f, 0.76f);
    scene->staticObjects.push_back(plane);
    
    RigidBody* ball = new RigidBody(new Model("models/ball.blend"), 0.0027f, 0.001027f, 0.75f);
    ball->scale = glm::vec3(0.04f);
    ball->initialPosition = glm::vec3(-2, 1, 0);
    ball->initialLinearVelocity = glm::vec3(3, 3, 0);
    ball->initialAngularVelocity = glm::vec3(0, 0, -6.28f);
    scene->dynamicObjects.push_back(ball);

    scene->initialize();

    Shader* traceShader = new Shader("shaders/trace.vert", "shaders/trace.frag");
    trace = new Trace(traceShader, camera, ball);
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
        deltaTime = (currentFrame - lastFrame) * playbackSpeed;
        lastFrame = currentFrame;

        // clear frame
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        
        // reset flags
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        // update scene
        if (playing)
        {
            scene->update(deltaTime);
            if (traceTrajectory)
                trace->update();
        }

        // draw scne
        scene->draw();
        if (traceTrajectory)
            trace->draw();

        // draw GUI
        screen->drawContents();
        screen->drawWidgets();

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
    createScene();
    createGUI();
    run();

    delete trace;
    delete scene;
    delete screen;
    delete resetButton;
    delete playButton;
    delete window;
    return EXIT_SUCCESS;
}