#include "shader.h"
#include "camera.h"
#include "light.h"
#include "model.h"
#include "transform.h"
#include "rigidbody.h"
#include "scene.h"
#include "trace.h"
#include "fileReader.h"

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
const float cameraScrollSpeed = 0.05f;
bool playing = false;
bool traceTrajectory;

// Automation attributes
const float automationIterationTime = 3.0f;
bool automating = false;
// Data entries read from file use to automate the simulation
vector<fileReader::DataEntry> dataEntries;
// Start time of the current iteration of the automation
float iterationStartFrame = 0;
int currentDataEntryIndex = 0;
/**
* The values recorded during automation of the data entry iterations, to be used for 
* mean absolute percentage deviation (MAPD) analysis with the model data.
* First value should be peakHeight, second value should be horizontalDisplacementAtBounce
**/
vector<pair<float, float>> automationValuesToCompare;
float peakHeightMAPD = 0;
float horizontalDistMAPD = 0;

// Drag coeficient constants
const float drag_ChoLeutheusser = 0.000207f;
const float drag_AdairGiordano = 0.00041f;
// The John Wesson coefficient needs to be multiplied by (radius * crossSection) of the ball
const float drag_Wesson = 0.3f;
// These final drag coefficients should correspond to the ones found to most resemble the captured model
const float drag_OursPingPong = 0.0001f;
const float drag_OursSoccer = 0.45f;

GLFWwindow* window;
nanogui::ref<Screen> screen;
nanogui::ref<FloatBox<float>> speedBox;
nanogui::ref<Slider> speedSlider;
nanogui::ref<Button> playButton;
FormHelper *gui;

vector<string> presets = { "Ping Pong", "Soccer" };
unsigned int preset;
vector<Scene> scenes;
unique_ptr<Scene> scene;
unique_ptr<Trace> trace;

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
* Zoom in/out mouse wheel scroll event
**/
void mouseWheelScrollCallbackEvent(GLFWwindow* window, double xoffset, double yoffset)
{
	if(yoffset < 0)
		scene->camera.position.z -= cameraScrollSpeed;
	else
		scene->camera.position.z += cameraScrollSpeed;
}

/**
 * Create the application scenes.
 */
void createScenes()
{
    // shaders
    shared_ptr<Shader> shader = make_shared<Shader>("shaders/scene.vert", "shaders/scene.frag");
    shared_ptr<Shader> depthShader = make_shared<Shader>("shaders/depth.vert", "shaders/depth.frag");
    shared_ptr<Shader> traceShader = make_shared<Shader>("shaders/trace.vert", "shaders/trace.frag");

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
    trace = make_unique<Trace>(&scene->dynamicObjects[0], &scene->camera, traceShader);
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
	gui->refresh();
}

/**
 * Load a preset scene.
 */
void loadScene()
{
    scene = make_unique<Scene>(scenes[preset]);
    trace->clear();
    trace->target = &scene->dynamicObjects[0];
    trace->camera = &scene->camera;
}

/**
* Set initial values of the simulation to match the ones in the current data entry from file
**/
void prepareNextAutomationIteration()
{
	resetScene();
	playing = true;
	scene->dynamicObjects[0].initialize(dataEntries[currentDataEntryIndex]);
	iterationStartFrame = glfwGetTime();
	gui->refresh();
}

/**
* Mean absolute percentage deviation analysis performed over the model and automated simulation values of the
* peak height and horizontalDisplacementAtBounce, respectively.
* M = (100%/n) * abs((At-Ft)/At)); where At is the actual value and Ft is the forecasted one.
**/
void calculateMAPD() 
{
	peakHeightMAPD = 0;
	horizontalDistMAPD = 0;

	int n = 0;
	for(n; n < automationValuesToCompare.size(); n++)
	{
		peakHeightMAPD += abs((dataEntries[n].peakHeight - automationValuesToCompare[n].first) / dataEntries[n].peakHeight);
		horizontalDistMAPD += abs((dataEntries[n].horizontalDisplacement - automationValuesToCompare[n].second) / dataEntries[n].horizontalDisplacement);
	}

	peakHeightMAPD, horizontalDistMAPD *= (100 / n);
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
* Create a widget of a row of buttons to select between preset values for a float variable.
* togglableValues should be a pair of button captions and the value to set the variable to.
*/
nanogui::ref<Widget> createAttributeTogglerWidget(Widget* parent, std::vector<pair<string, float>> togglableValues, float *value, FormHelper *gui)
{
	AdvancedGridLayout* parentLayout = (AdvancedGridLayout*)parent->layout();
	if (parentLayout->rowCount() > 0)
		parentLayout->appendRow(5);
	nanogui::ref<Widget> attributeToggler = new Widget(parent);
	nanogui::ref<BoxLayout> layout = new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 3);
	attributeToggler->setLayout(layout);
	
	int i = 0;
	for (pair<string, float> var : togglableValues)
	{
		nanogui::ref<Button> box = new Button(attributeToggler, togglableValues[i].first);
		box->setCallback([togglableValues, value, i, gui] {
			*value = togglableValues[i].second;
			gui->refresh();
		});
		i++;
	}
	
	return attributeToggler;
}

/**
 * Create the application GUI.
 */
void createGUI()
{
    // create GUI
    
    screen = new Screen();
    screen->initialize(window, true);
    gui = new FormHelper(screen.get());
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

	vector<pair<string, float>> presetDragValues;
	if (preset == 0) {
		presetDragValues = 
		{
			pair<string, float>("Default", drag_OursPingPong),
			pair<string, float>("Cho-Leutheusser", drag_ChoLeutheusser),
			pair<string, float>("Adair-Giordano", drag_AdairGiordano)
		};
	}
	else {
		float radius = scene->dynamicObjects[0].scale.x / 2;
		float crossSection = glm::pi<float>()*radius*radius;

		presetDragValues = 
		{
			pair<string, float>("Default", drag_OursSoccer),
			pair<string, float>("Wesson", drag_Wesson * radius * crossSection)
		};
	}

	gui->addWidget("Preset drag constants", createAttributeTogglerWidget(optionsWindow, presetDragValues, &scene->dynamicObjects[0].drag, gui));
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
    gui->addVariable("Trace trajectory", traceTrajectory);
    gui->addVariable("Keep previous", Trace::keepPrevious);

	gui->addVariable("Peak height", ball.peakHeight, false);
	gui->addVariable("Peak horizontal velocity", ball.peakLinearVelocity.x, false);
	gui->addVariable("Peak vectical velocity", ball.peakLinearVelocity.y, false);
	gui->addVariable("Peak sideways velocity", ball.peakLinearVelocity.z, false);
	gui->addVariable("Horizontal travel", ball.horizontalDisplacementAtBounce, false);

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

	if (preset == 0) 
	{
		gui->addButton("Automate from file", [&ball]()
		{
			fileReader fr = fileReader();
			dataEntries = fr.getReadDataEntries();
			currentDataEntryIndex = 0;
			automationValuesToCompare.clear();
			automating = true;
			prepareNextAutomationIteration();
		});

		gui->addVariable("Peak height MAPD (%)", peakHeightMAPD, false);
		gui->addVariable("Horizontal travel MAPD (%)", horizontalDistMAPD, false);
	}

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

		RigidBody &ball = scene->dynamicObjects[0];

		// update scene
		if (playing)
		{
			effectiveDeltaTime += deltaTime * playbackSpeed;

			if (automating)
			{
				currentFrame = glfwGetTime();

				// If we've reached the end of this iteration of the automation...
				if (currentFrame - iterationStartFrame >= automationIterationTime)
				{
					currentDataEntryIndex++;

					// Record peak values for statistical analysis, compute new MAPD
					automationValuesToCompare.push_back(pair<float, float>(ball.peakHeight, ball.horizontalDisplacementAtBounce));
					calculateMAPD();

					// ... check to see if done with automating and stop...
					if (currentDataEntryIndex >= dataEntries.size())
					{
						resetScene();
						playing = false;
						automating = false;
					}
					// ... reset initial values to next iteration and play again
					else 
					{
						prepareNextAutomationIteration();
					}
				}
			}

			while (effectiveDeltaTime / timeStep >= 1)
			{
				if (traceTrajectory)
					trace->update();
				scene->update(timeStep);
				gui->refresh();
				effectiveDeltaTime -= timeStep;
			}
		}

        // draw scene
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
    createScenes();
    createGUI();
	glfwSetScrollCallback(window, mouseWheelScrollCallbackEvent);
    run();

    glfwTerminate();
    return EXIT_SUCCESS;
}