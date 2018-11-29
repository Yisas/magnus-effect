﻿#include "shader.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "transform.h"
#include "rigidbody.h"
#include "scene.h"
#include "trace.h"
#include "direction.h"

#include <nanogui/nanogui.h>
#include <iostream>
#include <memory>

using namespace nanogui;
using namespace std;

const string TITLE = "Magnus Effect";
int width = 1600, height = 900;
float lastFrame, deltaTime;
float effectiveDeltaTime = 0;
float timeStep = 0.01f;
float playbackSpeed = 1;
bool playing = false;
bool traceTrajectory = false;
bool traceDirection = false;

GLFWwindow* window;
nanogui::ref<Screen> screen;
nanogui::ref<FloatBox<float>> speedBox;
nanogui::ref<Slider> speedSlider;
nanogui::ref<Button> playButton;

vector<string> presets = { "Ping Pong", "Soccer" };
unsigned int preset;
vector<Scene> scenes;
unique_ptr<Scene> scene;
unique_ptr<Trace> trace;
unique_ptr<Direction> direction;

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
 * Create the application scenes.
 */
void createScenes()
{
    // shaders
    shared_ptr<Shader> shader = make_shared<Shader>("shaders/scene.vert", "shaders/scene.frag");
    shared_ptr<Shader> depthShader = make_shared<Shader>("shaders/depth.vert", "shaders/depth.frag");
    shared_ptr<Shader> lineShader = make_shared<Shader>("shaders/trace.vert", "shaders/trace.frag");

    // models
    shared_ptr<Model> planeModel = make_shared<Model>("models/plane.blend");
    shared_ptr<Model> ballModel = make_shared<Model>("models/ball.blend");

    // ping pong scene
    {
        Camera camera(width, height, 45);
        camera.position = glm::vec3(0.0f, 0.5f, -2.5f);
        Light light = Light();
        light.position = glm::vec3(0, 50, 0);
        Transform plane(planeModel);
        plane.rotation = glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(1, 0, 0));
        plane.scale = glm::vec3(2.74f, 1.0f, 1.525f);
        RigidBody ball(ballModel, 0.0027f, 0.75f, 0.0001f);
        ball.scale = glm::vec3(0.04f);
        ball.initialPosition = glm::vec3(-1.0f, 0.5f, 0.0f);
        ball.initialLinearVelocity = glm::vec3(1.5f, 1.5f, 0.0f);
        ball.initialAngularVelocity = glm::vec3(0, 0, -6.28f);
        Scene pingPongScene(camera, light, shader, depthShader);
        pingPongScene.staticObjects.push_back(plane);
        pingPongScene.dynamicObjects.push_back(ball);
        pingPongScene.initialize();
        scenes.push_back(pingPongScene);
    }

    // soccer scene
    {
        Camera camera(width, height, 45);
        camera.position = glm::vec3(0, 1, -23);
        Light light = Light();
        light.position = glm::vec3(0, 50, -15);
        Transform plane(planeModel);
        plane.rotation = glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(1, 0, 0));
        plane.scale = glm::vec3(50);
        RigidBody ball(ballModel, 0.450f, 0.5f, 0.005f);
        ball.scale = glm::vec3(0.22f);
        ball.initialPosition = glm::vec3(0, 0.11f, -20);
        ball.initialLinearVelocity = glm::vec3(5, 5, 20);
        ball.initialAngularVelocity = glm::vec3(-20, -40, 0);
        Scene pingPongScene(camera, light, shader, depthShader);
        pingPongScene.staticObjects.push_back(plane);
        pingPongScene.dynamicObjects.push_back(ball);
        scenes.push_back(pingPongScene);
    }

    scene = make_unique<Scene>(scenes[preset]);
    trace = make_unique<Trace>(&scene->dynamicObjects[0], &scene->camera, lineShader);
    direction = make_unique<Direction>(&scene->dynamicObjects[0], &scene->camera, lineShader);
}

/**
 * Reset the scene to its default state.
 */
void resetScene()
{
    playing = false;
    playButton->setCaption("Play");
    scene->initialize();
    trace->reset();
}

/**
 * Load a preset scene.
 */
void loadScene()
{
    scene = make_unique<Scene>(scenes[preset]);
    RigidBody *newBall = &scene->dynamicObjects[0];
    Camera *newCamera = &scene->camera;

    trace->clear();
    trace->target = newBall;
    trace->camera = newCamera;
    direction->target = newBall;
    direction->camera = newCamera;
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
                resetScene();
            });
        }
        else if (axis == "y")
        {
            box->setValue(vector->y);
            box->setCallback([=](float value)
            {
                vector->y = value;
                resetScene();
            });
        }
        else
        {
            box->setValue(vector->z);
            box->setCallback([=](float value)
            {
                vector->z = value;
                resetScene();
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
    FormHelper *gui = new FormHelper(screen.get());
    nanogui::ref<Window> optionsWindow = gui->addWindow(Eigen::Vector2i(20, 20), "Simulation");
    
    gui->addGroup("Preset");
    nanogui::ref<ComboBox> presetBox = new ComboBox(optionsWindow, presets);
    presetBox->setFontSize(16);    
    presetBox->setSelectedIndex(preset);
    presetBox->setCallback([&](const int value)
    {
        preset = value;
        loadScene();
        resetScene();
        createGUI();
    });
    gui->addWidget("Scene", presetBox);
    
    gui->addGroup("Ball");
    RigidBody &ball = scene->dynamicObjects[0];
    gui->addVariable<float>("Size",
        [=](float size) { scene->dynamicObjects[0].scale = glm::vec3(size); },
        [=]() { return scene->dynamicObjects[0].scale.x; }
    );
    gui->addVariable("Mass", scene->dynamicObjects[0].mass);
    gui->addVariable("Drag", scene->dynamicObjects[0].drag);
    gui->addVariable("Bounciness", scene->dynamicObjects[0].bounciness);
    gui->addWidget("Initial position", createVectorBox(optionsWindow, &ball.initialPosition));
    gui->addWidget("Linear velocity", createVectorBox(optionsWindow, &ball.initialLinearVelocity));
    gui->addWidget("Angular velocity", createVectorBox(optionsWindow, &ball.initialAngularVelocity));
    gui->addVariable("Gravitational force", RigidBody::useGravity);
    gui->addVariable("Magnus force", RigidBody::useMagnusForce);

    gui->addGroup("Rendering");
    Camera &camera = scene->camera;
    gui->addWidget("Camera position", createVectorBox(optionsWindow, &camera.position));
    gui->addWidget("Camera direction", createVectorBox(optionsWindow, &camera.direction));
    gui->addVariable("Display velocity", traceDirection);
    gui->addVariable("Trace trajectory", traceTrajectory);
    gui->addVariable("Keep previous", Trace::keepPrevious);

    gui->addGroup("Controls");
    speedBox = gui->addVariable("Playback speed", playbackSpeed);
    speedBox->setCallback([=](float value)
    {
        playbackSpeed = value;
        speedSlider->setValue(value);
    });
    speedSlider = new Slider(optionsWindow);
    speedSlider->setRange(pair<float, float>(0, 2));
    speedSlider->setValue(playbackSpeed);
    speedSlider->setCallback([=](float value)
    {
        playbackSpeed = value;
        speedBox->setValue(value);
    });
    gui->addWidget(" ", speedSlider);
    playButton = gui->addButton("Play", []()
    {
        if (!playing)
        {
            playing = true;
            playButton->setCaption("Stop");
        }
        else
        {
            resetScene();
        }
    });
    screen->setVisible(true);
    screen->performLayout();

    // event callbacks

    glfwSetCursorPosCallback(window,
        [](GLFWwindow *, double x, double y)
        {
            screen->cursorPosCallbackEvent(x, y);
        }
    );

    glfwSetMouseButtonCallback(window,
        [](GLFWwindow *, int button, int action, int modifiers)
        {
            screen->mouseButtonCallbackEvent(button, action, modifiers);
        }
    );

    glfwSetKeyCallback(window,
        [](GLFWwindow *, int key, int scancode, int action, int mods)
        {
            screen->keyCallbackEvent(key, scancode, action, mods);
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
            if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
            {
                if (playing)
                    resetScene();
                else
                    playButton->callback()();
            }
        }
    );

    glfwSetCharCallback(window,
        [](GLFWwindow *, unsigned int codepoint)
        {
            screen->charCallbackEvent(codepoint);
        }
    );

    glfwSetDropCallback(window,
        [](GLFWwindow *, int count, const char **filenames)
        {
            screen->dropCallbackEvent(count, filenames);
        }
    );

    glfwSetScrollCallback(window,
        [](GLFWwindow *, double x, double y)
        {
            screen->scrollCallbackEvent(x, y);
       }
    );

    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow *, int newWidth, int newHeight)
        {
            width = newWidth;
            height = newHeight;
            screen->resizeCallbackEvent(width, height);
            glViewport(0, 0, width, height);
            scene->camera.width = width;
            scene->camera.height = height;
        }
    );
}

/**
 * Run the application main loop.
 */
void run()
{
    // rendering loop
    lastFrame = glfwGetTime();
    while (glfwWindowShouldClose(window) == false)
    {
        float currentFrame = glfwGetTime();
        deltaTime = (currentFrame - lastFrame);
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
            effectiveDeltaTime += deltaTime * playbackSpeed;
            while (effectiveDeltaTime / timeStep >= 1)
            {
                if (traceTrajectory)
                    trace->update();
                scene->update(timeStep);
                effectiveDeltaTime -= timeStep;
            }
        }

        // draw scene
        scene->draw();
        if (traceTrajectory)
            trace->draw();
        if (traceDirection)
            direction->draw();

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
    createScenes();
    createGUI();
    run();

    glfwTerminate();
    return EXIT_SUCCESS;
}