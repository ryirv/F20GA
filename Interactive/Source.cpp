// 3D Graphics and Animation - Main Template
// This uses Visual Studio Code - https://code.visualstudio.com/docs/cpp/introvideos-cpp
// Two versions available -  Win64 and Apple MacOS - please see notes
// Last changed September 2023

//#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")

//#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;


#define GL_SILENCE_DEPRECATION
#include <glad/glad.h> // Extend OpenGL Specfication to version 4.5 for WIN64 and versions 4.1 for Apple (note: two different files).

#include <imgui/imgui.h>			  // Load GUI library - Dear ImGui - https://github.com/ocornut/imgui
#include <imgui/imgui_impl_glfw.h>	  // Platform ImGui using GLFW
#include <imgui/imgui_impl_opengl3.h> // Platform new OpenGL - aka better than 3.3 core version.

#include <GLFW/glfw3.h> // Add GLFW - library to launch a window and callback mouse and keyboard - https://www.glfw.org

#define GLM_ENABLE_EXPERIMENTAL	 // Enable all functions include non LTS
#include <glm/glm.hpp>			 // Add helper maths library - GLM 0.9.9.9 - https://github.com/g-truc/glm - for example variables vec3, mat and operators.
#include <glm/gtx/transform.hpp> // Help us with transforms
using namespace glm;

//#include <tinygltf/tiny_gltf.h> // Model loading library - tiny gltf - https://github.com/syoyo/tinygltf
//#include "src/stb_image.hpp" // Add library to load images for textures

//#include "src/Mesh.hpp" // Simplest mesh holder and OBJ loader - can update more - from https://github.com/BennyQBD/ModernOpenGLTutorial

#include "src/Pipeline.hpp"		// Setup pipeline and load shaders.
#include "src/Content.hpp"		// Setup content loader and drawing functions - https://github.com/KhronosGroup/glTF - https://github.com/syoyo/tinygltf 
#include "src/Debugger.hpp"		// Setup debugger functions.
#include "src/DynamicModel.hpp" // Setup dynamic model functions.

#define HALF_PI 1.57f
#define BASE_FRAMERATE 60.f
#define PICKUP_BUTTON GLFW_MOUSE_BUTTON_RIGHT 

#define HOTDOG_ID 2
#define BURGER_ID 3
#define FRIES_ID 4
#define KETCHUP_ID 5
#define MUSTARD_ID 6
#define ICECREAM_ID 7
#define PIZZA_ID 8
#define SPLAT_1_ID 9
#define SPLAT_2_ID 10
#define SPLAT_3_ID 11
#define SPLAT_4_ID 12
#define SPLAT_5_ID 13
#define SCENE_ID 14


vector<DynamicModel*> allModels;

// Main fuctions
void startup();
void updateInput();
void update();
void render();
void ui();
void endProgram();

// HELPER FUNCTIONS OPENGL
void hintsGLFW();
//string readShader(string name);
//void checkErrorShader(GLuint shader);
inline void errorCallbackGLFW(int error, const char *description){cout << "Error GLFW: " << description << "\n";};
void debugGL();

void APIENTRY openGLDebugCallback(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const GLvoid *userParam);
GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// Setup all the message loop callbacks - do this before Dear ImGui
// Callback functions for the window and interface devices
void onResizeCallback(GLFWwindow *window, int w, int h);
void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow *window, double x, double y);
void onMouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset);

// VARIABLES
GLFWwindow *window; 								// Keep track of the window
auto windowWidth = 800;								// Window width					
auto windowHeight =800;								// Window height
auto running(true);							  		// Are we still running our main loop
mat4 projMatrix;							 		// Our Projection Matrix
// vec3 cameraPosition = vec3(18.1f, 5.1f, 1.8f);		// Where is our camera
vec3 cameraPosition = vec3(0.f, 0.f, 0.f);
// float cameraYaw = -1.6;								// Yaw camera is looking
float cameraYaw = 0.;
float cameraPitch = 0.;								// Pitch camera is looking
vec3 cameraFront = vec3(0.0f, 0.0f, 0.0f);			// Camera front vector
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);				// Camera up vector
auto aspect = (float)windowWidth / (float)windowHeight;	// Window aspect ration
auto fovy = 45.0f;									// Field of view (y axis)
bool keyStatus[1024];								// Track key strokes
auto currentTime = 0.0f;							// Framerate
auto deltaTime = 0.0f;								// time passed
auto lastTime = 0.0f;								// Used to calculate Frame rate
auto xmouse = 0.0;
auto ymouse = 0.0;
DynamicModel* carryingItem = nullptr;
Pipeline pipeline;									// Add one pipeline plus some shaders.
Debugger debugger;									// Add one debugger to use for callbacks ( Win64 - openGLDebugCallback() ) or manual calls ( Apple - glCheckError() ) 

float lightX = 1.0f;
float lightY = 1.0f;
float lightZ = 1.0f;

float lightR = 1.0f;
float lightG = -10.0f;
float lightB = 1.0f;

int heldMouseButton = -1;

//Code for Displaying Descriptions
std::string describe;
bool showDesc = false;
auto startTime = 0.0f;
auto resetTime = 0.0f;
// Clicking on item
bool clicked = false;
bool clickedOnce = false;
vector<int> readd; //Pop up only appears once.
int main()
{
	cout << endl << "===" << endl << "3D Graphics and Animation - Running..." << endl;

	if (!glfwInit()) // Check if GLFW is working, if not then stop all
	{
		cout << "Could not initialise GLFW...";
		return -1;
	} 

	glfwSetErrorCallback(errorCallbackGLFW); // Setup a function callback to catch and display all GLFW errors.
	hintsGLFW();							 // Setup glfw with various hints.

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor()); // Grab reference to monitor
	// windowWidth = mode->width; windowHeight = mode->height; //fullscreen
	// window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL); // fullscreen

	// Create our Window
	const auto windowTitle = "Banana Crime Scene Investigation Cleanup"s;
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);
	if (!window) // Test if Window or OpenGL context creation failed
	{
		cout << "Could not initialise GLFW...";
		glfwTerminate();
		return -1;
	} 

	glfwSetWindowPos(window, 10, 10); // Place it in top corner for easy debugging.
	glfwMakeContextCurrent(window);	  // making the OpenGL context current

	// GLAD: Load OpenGL function pointers - aka update specs to newest versions - plus test for errors.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD...";
		glfwMakeContextCurrent(NULL);
		glfwTerminate();
		return -1;
	}

	glfwSetWindowSizeCallback(window, onResizeCallback);	   // Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);				   // Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback); // Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);	   // Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);	   // Set callback for mouse wheel.
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor Fullscreen
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS fullscreen.

	// Setup Dear ImGui and add context	-	https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); //(void)io;
								  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsLight(); // ImGui::StyleColorsDark(); 		// Setup Dear ImGui style

	// Setup Platform/Renderer ImGui backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const auto glsl_version = "#version 410";
	ImGui_ImplOpenGL3_Init(glsl_version);

	#if defined(__WIN64__)
		debugGL(); // Setup callback to catch openGL errors.	V4.2 or newer
	#elif(__APPLE__)
		glCheckError(); // Old way of checking for errors. Newest not implemented by Apple. Manually call function anywhere in code to check for errors.
	#endif

	glfwSwapInterval(1);			 // Ony render when synced (V SYNC) - https://www.tomsguide.com/features/what-is-vsync-and-should-you-turn-it-on-or-off
	glfwWindowHint(GLFW_SAMPLES, 2); // Multisampling - covered in lectures - https://www.khronos.org/opengl/wiki/Multisampling

	startup(); // Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	cout << endl << "Starting main loop and rendering..." << endl;	

	do{											 // run until the window is closed
		auto currentTime = (float)glfwGetTime(); // retrieve timelapse
		deltaTime = currentTime - lastTime;		 // Calculate delta time
		lastTime = currentTime;					 // Save for next frame calculations.

		glfwPollEvents(); 						// poll callbacks

		updateInput();
		update(); 								// update (physics, animation, structures, etc)
		render(); 								// call render function.
		ui();									// call function to render ui.

		#if defined(__APPLE__)
			glCheckError();				// Manually checking for errors for MacOS, Windows has a callback.
		#endif

		glfwSwapBuffers(window); 		// swap buffers (avoid flickering and tearing)

		glfwGetCursorPos(window, &xmouse, &ymouse);
		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE); // exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);
	endProgram(); // Close and clean everything up...

	// cout << "\nPress any key to continue...\n";
	// cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}


void hintsGLFW(){
	
	auto majorVersion = 3; auto minorVersion = 3; // define OpenGL version - at least 3.3 for bare basic NEW OpenGL

	#if defined(__WIN64__)	
		majorVersion = 4; minorVersion = 5;					// Recommended for Windows 4.5, but latest is 4.6 (not all drivers support 4.6 version).
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Create context in debug mode - for debug message callback
	#elif(__APPLE__)
		majorVersion = 4; minorVersion = 1; 				// Max support for OpenGL in MacOS
	#endif

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 	// https://www.glfw.org/docs/3.3/window_guide.html
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
}

void endProgram()
{
	// Clean ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwMakeContextCurrent(NULL); 	// destroys window handler
	glfwTerminate();				// destroys all windows and releases resources.
}

Pipe materialBasics;
Pipe spinningLight;
Pipe hoverShader;

void startup()
{
	// Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;	

	cout << endl << "Loading content..." << endl;

	//add the burger:
	//create a new wrapper for the model we are adding so we can access the Content object and the model's properties from one place
	DynamicModel* burger = new DynamicModel(
		BURGER_ID,
		vec3(4.6f, 0.1f, 100.7f),  // Position
		vec3(0.0f, 0.0f, 0.0f),  // Rotation
		vec3(0.5f, 0.5f, 0.5f),  // Scale
		"assets/burger/burger.gltf",
		"assets/burger/burger.png"
		);
	allModels.push_back(burger);

	//fries next:
	//create a new wrapper for the model we are adding so we can access the Content object and the model's properties from one place
	// DynamicModel* fries = new DynamicModel(
	// 	PIZZA_ID,
	// 	vec3(-4.8f, 0.1f, 104.6f),  // Position
	// 	vec3(0.0f, 0.0f, 0.0f),  // Rotation
	// 	vec3(1.3f, 1.3f, 1.3f),  // Scale
	// 	"assets/fries/fries.gltf",
	// 	"assets/textures/Fries.png"
	// );
	// allModels.push_back(fries);
	
	
	
	
	DynamicModel* pizza = new DynamicModel(
		PIZZA_ID,
		vec3(-4.8f, 0.1f, 104.6f),  // Position
		vec3(0.0f, 0.0f, 0.0f),  // Rotation
		vec3(0.7f, 0.7f, 0.7f),  // Scale
		"assets/pizza/pizza.gltf",
		"assets/textures/Pizza.png"
	);

	// add the model details of the fries to the list of all models in the scene so it can be rendered and so that we can manipulate its scale/position/rotation properties later (in response to user input)
	allModels.push_back(pizza);

	//Now add the rest of the models here...
	//add the hotdog:
	//create a new wrapper for the model we are adding so we can access the Content object and the model's properties from one place
	DynamicModel* hotdog = new DynamicModel(
		HOTDOG_ID,
		vec3(-3.1f, 0.3f, 98.8f),   // Position
		vec3(0.0f, 0.0f, 0.0f),   // Rotation
		vec3(0.35f, 0.35f,0.35f),    // Scale
		"assets/hotdog/hotdog.gltf",
		"assets/textures/Hotdog.png"
	);
	// add the model details of the hotdog to the list of all models in the scene so it can be rendered and so that we can manipulate its scale/position/rotation properties later (in response to user input)
	allModels.push_back(hotdog);


	//add the Ketchup Condiment:
	//create a new wrapper for the model we are adding so we can access the Content object and the model's properties from one place
	DynamicModel* ketchup = new DynamicModel(
		KETCHUP_ID,
		vec3(4.6f, 0.1f, 96.2f),   // Position
		vec3(0.0f, 0.0f, 0.0f),   // Rotation
		vec3(0.3f, 0.3f,0.3f),    // Scale
		"assets/condimentBottles/ketchup_condiment.gltf",
		"assets/textures/Condiment_Ketchup.png"
	);
	// add the model details of the ketchup condiment to the list of all models in the scene so it can be rendered and so that we can manipulate its scale/position/rotation properties later (in response to user input)
	allModels.push_back(ketchup);


	//add the Mustard Condiment:
	//create a new wrapper for the model we are adding so we can access the Content object and the model's properties from one place

	DynamicModel* mustard = new DynamicModel(
		MUSTARD_ID,
		vec3(-0.6f, 0.1f, 96.0f),   // Position
		vec3(0.0f, 0.0f, 0.0f),   // Rotation
		vec3(0.3f, 0.3f,0.3f),    // Scale
		"assets/condimentBottles/mustard_condiment.gltf",
		"assets/textures/Condiment_Mustard.png"
	);
	// add the model details of the ketchup condiment to the list of all models in the scene so it can be rendered and so that we can manipulate its scale/position/rotation properties later (in response to user input)
	allModels.push_back(mustard);
  
  
	DynamicModel* baseScene = new DynamicModel(
		SCENE_ID,
		vec3(0.0f, 0.0f, 100.0f),   // Position
		vec3(0.0f, 0.0f, 0.0f),   // Rotation
		vec3(1.0f, 1.0f,1.0f),    // Scale
		"assets/base_model-nofood_copy_copy.gltf", 
    "assets/textures/Scene.png"
	);
  
  // WARNING: Base scene is hoverable by player!
  // Need a patch to prevent it from being hoverable!
	allModels.push_back(baseScene);


	// Load our shaders
	materialBasics = pipeline.LoadShaders("shaders/vs_model.glsl","shaders/fs_material.glsl");
	spinningLight = pipeline.LoadShaders("shaders/vs_model.glsl","shaders/fs_lights.glsl");
	hoverShader = pipeline.LoadShaders("shaders/vs_model.glsl","shaders/fs_hover.glsl");
	pipeline.UsePipe(&materialBasics);

	// A few optimizations.
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	
	// A few disoptimizations (sorry not sorry)
	// Disabling this as an easy fix to our 3D models
	// which have some of the normals flipped.
	glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Get the correct size in pixels - E.g. if retina display or monitor scaling
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	projMatrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}





















int keyHold[1024];
int clickHold = 0;

bool keyPressedOnce(int key) {
	return keyStatus[key] && keyHold[key] == 1;
}

void updateInput() {
	for (int i = 0; i < 1024; i++) {
		if (keyStatus[i]) {
			keyHold[i]++;
		}
		else keyHold[i] = 0;
	}
	if (clicked) {
		clickHold++;
	}
	else {
		clickHold = 0;
	}
}

float delta = 0.f;

float getLiveFPS() {
	float timeframe = 1.f/BASE_FRAMERATE;
	return (timeframe/(float)deltaTime)*BASE_FRAMERATE;
}

void updateDelta() {
	delta = BASE_FRAMERATE/getLiveFPS();
}


void updateObjectHover() {
	// Update static variables so that DynamicModels can access them

	const float PI = 3.1415926535897932384626433832795f;
    const float H_PI = 1.5707963267948966192313216916398f;

	DynamicModel::cameraX = cameraPosition.x;
	DynamicModel::cameraY = cameraPosition.y;
	DynamicModel::cameraZ = cameraPosition.z;

	DynamicModel::cameraYaw = cameraYaw;
	DynamicModel::cameraPitch = cameraPitch;

	DynamicModel::flatSinDirectionYaw = sin(cameraYaw+H_PI);
  	DynamicModel::flatCosDirectionYaw = cos(cameraYaw+H_PI);

  	DynamicModel::flatSinDirectionPitch = sin((cameraPitch-PI));
  	DynamicModel::flatCosDirectionPitch = cos((cameraPitch-PI));

	// Reset these
	// Make closestval biggest float number
	DynamicModel::closestVal = FLT_MAX;
	DynamicModel::closestObject = NULL;

	// Step 1: calculate each of its distances from the player.
	for (DynamicModel* o : allModels) {
    	o->calculateCloseness();
	}
	// Step 2: check if we're hovering, and if the closest, claim the
	// spot of the closest object.
	for (DynamicModel* o : allModels) {
		o->checkHovering();
	}

	//for (DynamicModel* o : allModels) {
		//if (o->isHovering()) ;
	//}
}

float accelerate = 0.f;

void updateWalk() {
	

	float movex = 0.;
	float movez = 0.;
	float rot = 0.;

	float sin_d = sin(cameraYaw);
	float cos_d = cos(cameraYaw);

	// NOTE: isWalking not used but could be useful later.
	bool isWalking = false;

	// NOTE: Delta here corrects the speed if we're experiencing
	// frame drops.

	// NOTE: same with speed.
	float speed = 0.1f*delta;

	if (keyStatus[GLFW_KEY_TAB]) {
		speed = (0.2f+accelerate)*delta;
		accelerate += 0.02f;
		if (accelerate > 1.f) accelerate = 1.f;
	}
	else accelerate = 0.f;

	// Forward
	if (keyStatus[GLFW_KEY_W]) {
		movex += sin_d*speed;
		movez += cos_d*speed;

		isWalking = true;
	}
	// Left
	if (keyStatus[GLFW_KEY_A]) {
		movex += cos_d*speed;
		movez += -sin_d*speed;

		isWalking = true;
	}
	// Backwards
	if (keyStatus[GLFW_KEY_S]) {
		movex += -sin_d*speed;
		movez += -cos_d*speed;

		isWalking = true;
	}
	// Right
	if (keyStatus[GLFW_KEY_D]) {
		movex += -cos_d*speed;
		movez += sin_d*speed;
		isWalking = true;
	}

	// Up and down
	if (keyStatus[GLFW_KEY_SPACE] || keyStatus[GLFW_KEY_R]) cameraPosition.y += speed;
	if (keyStatus[GLFW_KEY_LEFT_SHIFT] || keyStatus[GLFW_KEY_LEFT_CONTROL] || keyStatus[GLFW_KEY_F]) cameraPosition.y -= speed;

	// Teo's turn controls lol
	if (keyStatus[GLFW_KEY_Q]) cameraYaw += 0.05f*delta;
	if (keyStatus[GLFW_KEY_E]) cameraYaw -= 0.05f*delta;

	cameraPosition.x += movex;
	cameraPosition.z += movez;	
}

void updateTurnCamera(double deltaX,double deltaY) {
	// Note: later we could possibly change this to the mouse pointer
	// to control the direction of the camera.
	float TURN_SPEED = 0.0007f*delta;

	// TODO: re-add delta time with accurate equasion.
	cameraYaw += deltaX * TURN_SPEED;
	cameraPitch += deltaY * TURN_SPEED;
	// Do not allow us to look up more than we should otherwise things will
	// go really weird
	if (cameraPitch > HALF_PI) cameraPitch = HALF_PI;
	if (cameraPitch < -HALF_PI) cameraPitch = -HALF_PI;
}




void setLightPos(float x, float y, float z) {
	glUniform3f(glGetUniformLocation(spinningLight.program, "light_direction"), x, y, z);
}

void setViewPos(float x, float y, float z) {
	glUniform3f(glGetUniformLocation(spinningLight.program, "view_direction"), x, y, z);
}

bool murraysCameraMode = true;


void update()
{
	updateDelta();
	updateWalk();


	setViewPos(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	
	updateObjectHover();

	if (keyStatus[GLFW_KEY_LEFT_ALT]) pipeline.ReloadShaders();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if(carryingItem!=nullptr){
		float SELECT_FAR = 5.0;
		//stick infront of player
		float x = cameraPosition.x+sin(cameraYaw)*SELECT_FAR *cos(cameraPitch);
		float y = cameraPosition.y+sin(cameraPitch)*SELECT_FAR;
		float z = cameraPosition.z+cos(cameraYaw)*SELECT_FAR *cos(cameraPitch);

		float elapsedTime = (float)glfwGetTime()-startTime;
		int timeInMilliseconds = floor(elapsedTime * 1000.f);
		carryingItem->setPosition(vec3(x,y+sin(timeInMilliseconds * 0.001f),z));

		// Make the item spIiiiiiIIIIiiiiiIIIIn
		carryingItem->rot.y += 0.1f*delta;

		// if(heldMouseButton = PICKUP_BUTTON && (lastTime-startTime>1)){
		// If mouse released plonk down model.
		if(!clicked){
			carryingItem = nullptr;
			// resetTime = (float)glfwGetTime();
			printf("Resetting");
		}
	}
	if(showDesc){
		ImGui::Begin("Woah! Fun Facts!", &showDesc);
        ImGui::Text("%s",describe.c_str());
        ImGui::End();
	}
}

void updateDesc()
{
	bool found = false;
	for(int i = 0;i<readd.size();i++){
		if (readd[i]==carryingItem->id){
			found = true;
			break;
		} 
	}
	if(!found){
	// https://www.youtube.com/watch?v=q97E8rOFWSU
		ifstream inputFile;
		std::ostringstream oss;
		oss << "assets/descriptions/" << carryingItem->id << ".txt";
		std::string fileName = oss.str();
		inputFile.open(fileName.c_str());
		stringstream buffer;
		buffer << inputFile.rdbuf();
		describe = buffer.str();
		cout << describe;
		inputFile.close();
		printf("%s",describe);
		showDesc = true;
		readd.push_back(carryingItem->id);
	}
}

void render()
{
	glViewport(0, 0, windowWidth, windowHeight);

	// Clear colour buffer
	glm::vec4 grayLilac = glm::vec4(0.831f, 0.792f, 0.803f, 1.0f);
	glm::vec4 backgroundColor = grayLilac;
	glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear deep buffer
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//I've temporarily commented his camera code out. I just needed it to point to 0,0,0 to make sure that the objects we appearing as they should without fighting with the camera - Dean
	// Ive added a mode for disabling it lol. Just press 2 on your keyboard. - Teo
	vec3 lookAt;
	float x = cameraPosition.x;
	float y = cameraPosition.y;
	float z = cameraPosition.z;
	float LOOK_DIST = 50.f;
		
	//THIS WEIRD MATHS EQUASION EXPLAINED:
	// When we look at a point, this point should be restraint to a sphere around the camera.
	// The closer we look up/down, the closer to the centre of the y-axis the point should be. (cos(cameraYaw))
	// If we look straight forward (and not up or down), then the point is on the xz-plane.
	lookAt = vec3(x+sin(cameraYaw)*LOOK_DIST*cos(cameraPitch), y+sin(cameraPitch)*LOOK_DIST, z+cos(cameraYaw)*LOOK_DIST*cos(cameraPitch));
	


	//Setup camera to look at the origin (0,0,0)
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, // camera positiion
                                   lookAt,   // point in space that the camera is looking at
                                   vec3(0.0, 1.0, 0.0)); // z axis is "up"

	int l = allModels.size();
	for (std::size_t i = 0; i < l; ++i) {
    	DynamicModel* model = allModels[i];
		// Do some translations, rotations and scaling
		// glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(modelPosition.x+rX, modelPosition.y+rY, modelPosition.z+rZ));

		// I fully acknowledge that this isn't the most efficient approach.
		// However, we don't have much time;.
		bool usingmaterialbasics = false;
		if (model->isHovering()) {
			// If we're hovering, then we want to render the model with a different shader.
			pipeline.UsePipe(&hoverShader);
			glUniform1f(glGetUniformLocation(hoverShader.program, "u_time"), (float)glfwGetTime());
		}
		else if (carryingItem!=nullptr && carryingItem == model){
			pipeline.UsePipe(&materialBasics);
			float elapsedTime = (float)glfwGetTime()-startTime;
			int timeInMilliseconds = std::floor(elapsedTime * 1000);
      		float flashIntensity = std::sin(timeInMilliseconds * 0.002f) * 0.3f + 0.5f;
			cout << "flash intensity: " << flashIntensity << endl;
			glUniform1f(glGetUniformLocation(materialBasics.program, "in_flashIntesity"), flashIntensity);
			usingmaterialbasics = true;
		}
		else {
			// Otherwise, we want to render the model with the default shader.
			pipeline.UsePipe(&materialBasics);
			glUniform1f(glGetUniformLocation(materialBasics.program, "in_flashIntesity"), 0.f);
			usingmaterialbasics = true;
		}

		//apply translations
		mat4 modelMatrix = glm::translate(glm::mat4(1.0f), model->pos);

		//apply x-axis rotation
		modelMatrix = glm::rotate(modelMatrix, model->rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
		//apply y-axis rotation
		modelMatrix = glm::rotate(modelMatrix, model->rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
		//apply z-axis rotation
		modelMatrix = glm::rotate(modelMatrix, model->rot.z, glm::vec3(0.0f, 0.0f, 1.0f));

		//apply x-axis scale:
		modelMatrix = glm::scale(modelMatrix, model->scl);

		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(pipeline.currentPipe->program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.currentPipe->program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.currentPipe->program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);

		glBindTexture(GL_TEXTURE_2D, model->content.texid);

		// Temp code to highlight hovering model.
		if (model->isHovering() && clickHold == 1 && carryingItem == nullptr /*&& (((lastTime-resetTime)>1)||(resetTime==0.0f))*/) {
			// startTime = (float)glfwGetTime();
			if(model->id != SCENE_ID)
			{
				carryingItem = model;
				printf("Model Id %d", model->id);
				updateDesc();
			}
		}

		if (usingmaterialbasics) {
			glUniform3f(glGetUniformLocation(materialBasics.program, "in_ambient"), 0.7f, 0.7f, 0.7f);
			glUniform3f(glGetUniformLocation(materialBasics.program, "in_diffuse"), 0.7f, 0.7f, 0.7f);
			glUniform3f(glGetUniformLocation(materialBasics.program, "in_specular"), 0.1f, 0.1f, 0.1f);
			glUniform1f(glGetUniformLocation(materialBasics.program, "in_shininess"), 2.0f);
			glUniform3f(glGetUniformLocation(materialBasics.program, "in_lightColor"), 1.0f, 1.0f, 1.0f);
		}
		
		
		model->content.DrawModel(model->content.vaoAndEbos, model->content.model);
	}
	
	#if defined(__APPLE__)
		glCheckError();
	#endif
}

void ui()
{
	ImGuiIO &io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration; 
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	window_flags |= ImGuiWindowFlags_NoSavedSettings; 
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing; 
	window_flags |= ImGuiWindowFlags_NoNav;

	const auto PAD = 10.0f;
	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + work_size.y - PAD;
	window_pos_pivot.x = 1.0f;
	window_pos_pivot.y = 1.0f;

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	bool *p_open = NULL;
	if (ImGui::Begin("Info", nullptr, window_flags)) {
		ImGui::Text("About: Banana Crime Scene Investigation Cleanup; 2023/24"); // ImGui::Separator();
		ImGui::Text("Performance: %.3fms/Frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		// ImGui::Text("Pipeline: %s", pipeline.pipe.error?"ERROR":"OK");
		ImGui::Text("Camera: %.1f, %.1f, %.1f, %.1f, %.1f", cameraPosition.x, cameraPosition.y, cameraPosition.z, cameraYaw,cameraPitch);
	}
	ImGui::End();

	// Rendering imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void onResizeCallback(GLFWwindow *window, int w, int h)
{
	windowWidth = w;
	windowHeight = h;

	// Get the correct size in pixels
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	if (windowWidth > 0 && windowHeight > 0)
	{ // Avoid issues when minimising window - it gives size of 0 which fails division.
		aspect = (float)w / (float)h;
		projMatrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
	}
}

void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		keyStatus[key] = true;
	else if (action == GLFW_RELEASE)
		keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT)) {
		if(action == GLFW_PRESS){
			heldMouseButton = button;
			// std::cout << "Button Held" << std::endl;
		}
		else if (action == GLFW_RELEASE) {
			if(button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT){
				heldMouseButton = -1;
				// std::cout << "Button Released" << std::endl;
			}
    	}	
    }
	if ((button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)){
		clicked = true;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
		clicked = false;
	}
	
}

void onClickReleased(int button){
	if(button == GLFW_MOUSE_BUTTON_LEFT){
		//if mouse is over an object when left mouse button is released, detect which object

	}
}

int prevMouseX = 0;
int prevMouseY = 0;

void onMouseMoveCallback(GLFWwindow *window, double x, double y)
{
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);
	
	auto xChange = mouseX - prevMouseX;
	auto yChange = mouseY - prevMouseY;
	prevMouseX = mouseX;
	prevMouseY = mouseY;
	//If right clock is held down then we do camera movements
	if (murraysCameraMode) {
		if (heldMouseButton != -1) updateTurnCamera(xChange,yChange); 
	}
	else  updateTurnCamera(xChange,yChange);
		
}

void onMouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	int yoffsetInt = static_cast<int>(yoffset);
}

void APIENTRY openGLDebugCallback(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const GLvoid *userParam)  // Debugger callback for Win64 - OpenGL versions 4.2 or better.
{
	debugger.OpenGLDebugCallback(source, type, id, severity, length, message, userParam);
}

void debugGL() // Debugger function for Win64 - OpenGL versions 4.2 or better.
{
	// Enable Opengl Debug
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

GLenum glCheckError_(const char *file, int line) // Debugger manual function for Apple - OpenGL versions 3.3 to 4.1.
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) debugger.GlGetError(errorCode, file, line);

	return errorCode;
}
