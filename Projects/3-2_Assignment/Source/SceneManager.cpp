///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	// matrix math for calculating the final model matrix
	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		// pass the model matrix into the shader
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		// pass the color values into the shader
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadSphereMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadConeMesh();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	/*** FLOOR PLANE                                                ***/
	/******************************************************************/
	// Set the XYZ scale for the floor - wide and flat
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);

	// No rotation needed for the floor
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Position at origin (ground level)
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	// Apply transformations and set dark blue color for floor
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.4f, 0.5f, 0.8f, 1.0f);

	// Draw the floor plane
	m_basicMeshes->DrawPlaneMesh();

	/******************************************************************/
	/*** BACKGROUND WALL PLANE                                      ***/
	/******************************************************************/
	// Set the XYZ scale for the background wall
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);

	// Rotate 90 degrees on X-axis to make vertical wall
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Position wall behind the scene
	positionXYZ = glm::vec3(0.0f, 9.0f, -10.0f);

	// Apply transformations and set lighter blue color for wall
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.5f, 0.6f, 0.9f, 1.0f);

	// Draw the background wall plane
	m_basicMeshes->DrawPlaneMesh();

	/******************************************************************/
	/*** LEFT PEDESTAL - Cylinder supporting purple sphere          ***/
	/******************************************************************/
	// Scale cylinder to be shorter
	scaleXYZ = glm::vec3(1.0f, 0.65f, 1.0f);

	// No rotation needed
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Position on the left side of the scene
	positionXYZ = glm::vec3(-2.0f, 0.0f, 0.0f);

	// Apply transformations and set light blue color
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.6f, 0.7f, 1.00f, 1.0f);

	// Draw the left cylinder pedestal
	m_basicMeshes->DrawCylinderMesh();

	/******************************************************************/
	/*** PURPLE SPHERE - Sits on top of left cylinder               ***/
	/******************************************************************/
	// Scale sphere to medium size
	scaleXYZ = glm::vec3(0.65f, 0.65f, 0.65f);

	// No rotation needed for sphere
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Position above left cylinder
	positionXYZ = glm::vec3(-2.0f, 1.3f, 0.0f);

	// Apply transformations and set pink-purple color
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.75f, 0.55f, 0.85f, 1.0f);

	// Draw the purple sphere
	m_basicMeshes->DrawSphereMesh();

	/******************************************************************/
	/*** CENTER PEDESTAL - Tall cylinder supporting yellow cone     ***/
	/******************************************************************/
	// Scale cylinder to be taller for center pedestal
	scaleXYZ = glm::vec3(1.0f, 2.0f, 1.0f);

	// No rotation needed
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Position at center of scene
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	// Apply transformations and set light blue color
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.6f, 0.7f, 1.00f, 1.0f);

	// Draw the center cylinder pedestal
	m_basicMeshes->DrawCylinderMesh();

	/******************************************************************/
	/*** YELLOW CONE - Sits on top of center cylinder               ***/
	/******************************************************************/
	// Scale cone to be tall and narrow
	scaleXYZ = glm::vec3(0.7f, 2.0f, 0.7f);

	// No rotation needed (cone points up by default)
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Position above center cylinder
	positionXYZ = glm::vec3(0.0f, 2.01f, 0.0f);

	// Apply transformations and set bright yellow color
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(1.0f, 1.0f, 0.1f, 1.0f);

	// Draw the yellow cone
	m_basicMeshes->DrawConeMesh();

	/******************************************************************/
	/*** RIGHT PEDESTAL - Medium cylinder supporting red box        ***/
	/******************************************************************/
	// Scale cylinder to medium height
	scaleXYZ = glm::vec3(1.0f, 1.4f, 1.0f);

	// No rotation needed
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Position on the right side of the scene
	positionXYZ = glm::vec3(2.0f, 0.0f, 0.0f);

	// Apply transformations and set light blue color
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.6f, 0.7f, 1.00f, 1.0f);

	// Draw the right cylinder pedestal
	m_basicMeshes->DrawCylinderMesh();

	/******************************************************************/
	/*** RED BOX - Sits on top of right cylinder                    ***/
	/******************************************************************/
	// Scale box to medium size
	scaleXYZ = glm::vec3(1.2f, 1.2f, 1.2f);

	// Rotate 30 degrees on Y-axis to angle the box
	XrotationDegrees = 0.0f;
	YrotationDegrees = 30.0f;
	ZrotationDegrees = 0.0f;

	// Position above right cylinder
	positionXYZ = glm::vec3(2.0f, 1.9f, 0.0f);

	// Apply transformations and set coral/salmon color
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.95f, 0.5f, 0.45f, 1.0f);

	// Draw the coral-colored box
	m_basicMeshes->DrawBoxMesh();
	/****************************************************************/
}