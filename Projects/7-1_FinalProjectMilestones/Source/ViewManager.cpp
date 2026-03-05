///////////////////////////////////////////////////////////////////////////////
// viewmanager.cpp
// ============
// manage the viewing of 3D objects within the viewport - camera, projection
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>    

// declarations of the global variables and defines
namespace
{
	// Variables for window width and height
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 800;
	const char* g_ViewName = "view";
	const char* g_ProjectionName = "projection";

	// camera object used for viewing and interacting with
	// the 3D scene
	Camera* g_pCamera = nullptr;

	// these variables are used for mouse movement processing
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	// time between current frame and last frame
	float gDeltaTime = 0.0f;
	float gLastFrame = 0.0f;

	// the following variable is false when orthographic projection
	// is off and true when it is on
	bool bOrthographicProjection = false;
}

/***********************************************************
 *  ViewManager()
 *
 *  The constructor for the class
 ***********************************************************/
ViewManager::ViewManager(
	ShaderManager* pShaderManager)
{
	// initialize the member variables
	m_pShaderManager = pShaderManager;
	m_pWindow = NULL;
	g_pCamera = new Camera();
	// default camera view parameters
	g_pCamera->Position = glm::vec3(-3.31f, 8.94f, 7.42f);
	g_pCamera->Front = glm::vec3(0.35f, -0.46f, -0.82f);
	g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
	g_pCamera->Zoom = 80;
	g_pCamera->MovementSpeed = 20;

	glm::vec3 front = glm::normalize(g_pCamera->Front);
	g_pCamera->Pitch = glm::degrees(asin(front.y));
	g_pCamera->Yaw = glm::degrees(atan2(front.z, front.x));
}

/***********************************************************
 *  ~ViewManager()
 *
 *  The destructor for the class
 ***********************************************************/
ViewManager::~ViewManager()
{
	// free up allocated memory
	m_pShaderManager = NULL;
	m_pWindow = NULL;
	if (NULL != g_pCamera)
	{
		delete g_pCamera;
		g_pCamera = NULL;
	}
}

/***********************************************************
 *  CreateDisplayWindow()
 *
 *  This method is used to create the main display window.
 ***********************************************************/
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
	GLFWwindow* window = nullptr;

	// try to create the displayed OpenGL window
	window = glfwCreateWindow(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		windowTitle,
		NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	// this callback is used to receive mouse moving events
	glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);

	// this callback is used to recieve mouse scroll events
	glfwSetScrollCallback(window, &ViewManager::Mouse_Scroll_Callback);

	// tell GLFW to capture all mouse events
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// enable blending for supporting tranparent rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pWindow = window;

	return(window);
}

/***********************************************************
 *  Mouse_Position_Callback()
 *
 *  This method is automatically called from GLFW whenever
 *  the mouse is moved within the active GLFW display window.
 ***********************************************************/
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
	// when the first mouse move event is received, this needs to be recorded so that
	// all subsequent mouse moves can correctly calculate the X position offset and Y
	// position offset for proper operation
	if (gFirstMouse)
	{
		gLastX = xMousePos;
		gLastY = yMousePos;
		gFirstMouse = false;
	}

	// calculate the X offset and Y offset values for moving the 3D camera accordingly
	float xOffset = xMousePos - gLastX;
	float yOffset = gLastY - yMousePos; // reversed since y-coordinates go from bottom to top

	// set the current positions into the last position variables
	gLastX = xMousePos;
	gLastY = yMousePos;

	// move the 3D camera according to the calculated offsets
	g_pCamera->ProcessMouseMovement(xOffset, yOffset);
}

/***********************************************************
 *  Mouse_Scroll_Callback()
 *
 *  This method is automatically called from GLFW whenever
 *  the mouse scroll wheel is used within the active GLFW
 *  display window. It adjusts the camera movement speed.
 ***********************************************************/
void ViewManager::Mouse_Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset)
{
	// adjust the camera movement speed based on scroll wheel input
	// positive yOffset = scroll up = decrease speed
	// negative yOffset = scroll down = increase speed
	if (NULL != g_pCamera)
	{
		g_pCamera->ProcessMouseScroll(yOffset);
	}
}

/***********************************************************
 *  ProcessKeyboardEvents()
 *
 *  This method is called each frame to process any keyboard
 *  events waiting in the event queue. Handles camera movement,
 *  looking direction, and switching between projection modes.
 *
 *  Movement Controls:
 *    W/S        - move camera forward/backward
 *    A/D        - pan camera left/right
 *    Q/E        - move camera up/down
 *    Arrow Keys - look left/right/up/down
 *    Mouse      - free look
 *
 *  Preset Views (direct):
 *    1          - orthographic front view
 *    2          - orthographic left side view
 *    3          - orthographic back view
 *    4          - orthographic right side view
 *    5          - orthographic top view
 *    6          - perspective front view
 *    7          - perspective front-left view
 *    8          - perspective front-right view
 *    9          - perspective back view
 *
 *  Cycling Controls:
 *    P          - cycle through perspective presets (4 views)
 *    O          - cycle through orthographic presets (5 views)
 *
 *    ESC        - close the window
 ***********************************************************/
void ViewManager::ProcessKeyboardEvents()
{
	// track previous press state for P and O to prevent
	// holding the key from firing multiple times per press
	static bool pWasPressed = false;
	static bool oWasPressed = false;

	// track which preset index we are on for each cycle group
	static int perspIndex = 0;
	static int orthoIndex = 0;

	// close the window if the escape key has been pressed
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_pWindow, true);
	}

	// if the camera object is null, then exit this method
	if (NULL == g_pCamera)
	{
		return;
	}

	// W/S - move the camera forward and backward along its front vector
	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(FORWARD, gDeltaTime);
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(BACKWARD, gDeltaTime);
	}

	// A/D - pan the camera left and right along its right vector
	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(LEFT, gDeltaTime);
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(RIGHT, gDeltaTime);
	}

	// Q/E - move the camera vertically up and down
	if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(UP, gDeltaTime);
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(DOWN, gDeltaTime);
	}

	// Arrow Left/Right - rotate the camera's yaw left and right
	if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		g_pCamera->ProcessMouseMovement(-50.0f, 0.0f);
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		g_pCamera->ProcessMouseMovement(50.0f, 0.0f);
	}

	// Arrow Up/Down - rotate the camera's pitch up and down
	if (glfwGetKey(m_pWindow, GLFW_KEY_UP) == GLFW_PRESS)
	{
		g_pCamera->ProcessMouseMovement(0.0f, 50.0f);
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		g_pCamera->ProcessMouseMovement(0.0f, -50.0f);
	}

	// helper lambda to set camera position, front vector, up vector,
	// and recalculate pitch/yaw so free-look stays consistent after
	// snapping to a preset view
	auto SetCameraView = [](glm::vec3 position, glm::vec3 front, glm::vec3 up) {
		g_pCamera->Position = position;
		g_pCamera->Front = front;
		g_pCamera->Up = up;
		glm::vec3 f = glm::normalize(front);
		g_pCamera->Pitch = glm::degrees(asin(f.y));
		g_pCamera->Yaw = glm::degrees(atan2(f.z, f.x));
		};

	// 1 - orthographic front view, looking straight down the -Z axis
	if (glfwGetKey(m_pWindow, GLFW_KEY_1) == GLFW_PRESS)
	{
		bOrthographicProjection = true;
		SetCameraView(glm::vec3(0.0f, 7.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	// 2 - orthographic left side view, looking in the +X direction
	if (glfwGetKey(m_pWindow, GLFW_KEY_2) == GLFW_PRESS)
	{
		bOrthographicProjection = true;
		SetCameraView(glm::vec3(-10.0f, 7.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	// 3 - orthographic back view, looking in the +Z direction
	if (glfwGetKey(m_pWindow, GLFW_KEY_3) == GLFW_PRESS)
	{
		bOrthographicProjection = true;
		SetCameraView(glm::vec3(0.0f, 7.0f, -10.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	// 4 - orthographic right side view, looking in the -X direction
	if (glfwGetKey(m_pWindow, GLFW_KEY_4) == GLFW_PRESS)
	{
		bOrthographicProjection = true;
		SetCameraView(glm::vec3(10.0f, 7.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	// 5 - orthographic top view, looking straight down the -Y axis
	if (glfwGetKey(m_pWindow, GLFW_KEY_5) == GLFW_PRESS)
	{
		bOrthographicProjection = true;
		SetCameraView(glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
	}
	// 6 - perspective view from the front at a slight downward angle
	if (glfwGetKey(m_pWindow, GLFW_KEY_6) == GLFW_PRESS)
	{
		bOrthographicProjection = false;
		SetCameraView(glm::vec3(0.0f, 8.5f, 8.0f), glm::vec3(0.0f, -1.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		g_pCamera->Zoom = 80;
	}
	// 7 - perspective view from the front-left diagonal
	if (glfwGetKey(m_pWindow, GLFW_KEY_7) == GLFW_PRESS)
	{
		bOrthographicProjection = false;
		SetCameraView(glm::vec3(-6.0f, 8.5f, 6.0f), glm::vec3(0.6f, -0.6f, -0.6f), glm::vec3(0.0f, 1.0f, 0.0f));
		g_pCamera->Zoom = 80;
	}
	// 8 - perspective view from the front-right diagonal
	if (glfwGetKey(m_pWindow, GLFW_KEY_8) == GLFW_PRESS)
	{
		bOrthographicProjection = false;
		SetCameraView(glm::vec3(6.0f, 8.5f, 6.0f), glm::vec3(-0.6f, -0.6f, -0.6f), glm::vec3(0.0f, 1.0f, 0.0f));
		g_pCamera->Zoom = 80;
	}
	// 9 - perspective view from behind the scene
	if (glfwGetKey(m_pWindow, GLFW_KEY_9) == GLFW_PRESS)
	{
		bOrthographicProjection = false;
		SetCameraView(glm::vec3(0.0f, 8.5f, -8.0f), glm::vec3(0.0f, -0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		g_pCamera->Zoom = 80;
	}

	// P - cycle forward through the 4 perspective preset views one press at a time
	if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (!pWasPressed)
		{
			bOrthographicProjection = false;
			perspIndex = (perspIndex + 1) % 4;

			if (perspIndex == 0)      // front
				SetCameraView(glm::vec3(0.0f, 8.5f, 8.0f), glm::vec3(0.0f, -1.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			else if (perspIndex == 1) // front-left
				SetCameraView(glm::vec3(-6.0f, 8.5f, 6.0f), glm::vec3(0.6f, -0.6f, -0.6f), glm::vec3(0.0f, 1.0f, 0.0f));
			else if (perspIndex == 2) // front-right
				SetCameraView(glm::vec3(6.0f, 8.5f, 6.0f), glm::vec3(-0.6f, -0.6f, -0.6f), glm::vec3(0.0f, 1.0f, 0.0f));
			else if (perspIndex == 3) // back
				SetCameraView(glm::vec3(0.0f, 8.5f, -8.0f), glm::vec3(0.0f, -0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			g_pCamera->Zoom = 80;
			pWasPressed = true;
		}
	}
	else if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_RELEASE)
	{
		// reset flag when key is released so next press registers as a new event
		pWasPressed = false;
	}

	// O - cycle forward through the 5 orthographic preset views one press at a time
	if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS)
	{
		if (!oWasPressed)
		{
			bOrthographicProjection = true;
			orthoIndex = (orthoIndex + 1) % 5;

			if (orthoIndex == 0)      // front
				SetCameraView(glm::vec3(0.0f, 7.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			else if (orthoIndex == 1) // left side
				SetCameraView(glm::vec3(-10.0f, 7.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			else if (orthoIndex == 2) // back
				SetCameraView(glm::vec3(0.0f, 7.0f, -10.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			else if (orthoIndex == 3) // top
				SetCameraView(glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
			else if (orthoIndex == 4) // right side
				SetCameraView(glm::vec3(10.0f, 7.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			oWasPressed = true;
		}
	}
	else if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_RELEASE)
	{
		// reset flag when key is released so next press registers as a new event
		oWasPressed = false;
	}
}

/***********************************************************
 *  PrepareSceneView()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void ViewManager::PrepareSceneView()
{
	glm::mat4 view;
	glm::mat4 projection;

	// per-frame timing
	float currentFrame = glfwGetTime();
	gDeltaTime = currentFrame - gLastFrame;
	gLastFrame = currentFrame;

	// process any keyboard events that may be waiting in the 
	// event queue
	ProcessKeyboardEvents();

	// get the current view matrix from the camera
	view = g_pCamera->GetViewMatrix();

	// define the current projection matrix
	if (bOrthographicProjection)
		projection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 100.0f);
	else
		projection = glm::perspective(glm::radians(g_pCamera->Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

	// if the shader manager object is valid
	if (NULL != m_pShaderManager)
	{
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ViewName, view);
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ProjectionName, projection);
		// set the view position of the camera into the shader for proper rendering
		m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
	}



	/*
	*	was used to set to a nice starting position for the camera
	*	but changes the window title to do so, commented out before
	*	submitting assignment
	* */
	/*
	if (NULL != m_pShaderManager && NULL != g_pCamera)
	{
		std::string title = "Camera - Pos: (" +
			std::to_string(g_pCamera->Position.x) + ", " +
			std::to_string(g_pCamera->Position.y) + ", " +
			std::to_string(g_pCamera->Position.z) + ") | Front: (" +
			std::to_string(g_pCamera->Front.x) + ", " +
			std::to_string(g_pCamera->Front.y) + ", " +
			std::to_string(g_pCamera->Front.z) + ") | Zoom: " +
			std::to_string(g_pCamera->Zoom);

		glfwSetWindowTitle(m_pWindow, title.c_str());
	}
	*/
}