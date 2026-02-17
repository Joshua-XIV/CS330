///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#include <glm/gtx/transform.hpp>

// declare the global variables
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
SceneManager::SceneManager(ShaderManager* pShaderManager)
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
	// free up the allocated memory
	m_pShaderManager = NULL;
	if (NULL != m_basicMeshes)
	{
		delete m_basicMeshes;
		m_basicMeshes = NULL;
	}
	// clear the collection of defined materials
	m_objectMaterials.clear();
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			//material.ambientColor = m_objectMaterials[index].ambientColor;
			//material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformation()
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

	// matrix math is used to calculate the final model matrix
	modelView = translation * rotationX * rotationY * rotationZ * scale;
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
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		// find the defined material that matches the tag
		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			// pass the material properties into the shader
			//m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			//m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

 /***********************************************************
  *  DefineObjectMaterials()
  *
  *  This method is used for configuring the various material
  *  settings for all of the objects within the 3D scene.
  ***********************************************************/
void SceneManager::DefineObjectMaterials()
{
	/*** STUDENTS - add the code BELOW for defining object materials. ***/
	/*** There is no limit to the number of object materials that can ***/
	/*** be defined. Refer to the code in the OpenGL Sample for help  ***/

	// Default material with pink tint
	OBJECT_MATERIAL defaultMat;
	//defaultMat.ambientColor = glm::vec3(0.8f, 0.2f, 0.2f);  // White
	//defaultMat.ambientStrength = 0.3f;
	defaultMat.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);  // White
	defaultMat.specularColor = glm::vec3(1.0f, 1.0f, 1.0f); // White
	defaultMat.shininess = 1.0f;
	defaultMat.tag = "default";
	m_objectMaterials.push_back(defaultMat);

	// material with high shininess for reflective surfaces
	OBJECT_MATERIAL metalMat;
	//metalMat.ambientColor = glm::vec3(0.5f, 0.2f, 0.2f);    // red ambient
	//metalMat.ambientStrength = 0.7f;
	metalMat.diffuseColor = glm::vec3(0.3f, 0.3f, 0.3f);    // light gray diffuse
	metalMat.specularColor = glm::vec3(0.8f, 0.8f, 0.8f);   // white specular
	metalMat.shininess = 32.0f;                            // very shiny
	metalMat.tag = "shiny";
	m_objectMaterials.push_back(metalMat);

	// material with medium shininess and slight color tint, will be dimmer than default
	// trying to resemble a plastic like material
	OBJECT_MATERIAL plasticMat;
	//plasticMat.ambientColor = glm::vec3(0.3f, 0.3f, 0.3f);  // gray ambient
	//plasticMat.ambientStrength = 0.3f;
	plasticMat.diffuseColor = glm::vec3(0.5f, 0.5f, 1.0f);  // blue tint
	plasticMat.specularColor = glm::vec3(0.8f, 0.8f, 0.9f); // blue-white specular
	plasticMat.shininess = 32.0f;                           // medium shininess
	plasticMat.tag = "plastic";
	m_objectMaterials.push_back(plasticMat);

	// material to resemble wood, low shine
	OBJECT_MATERIAL woodMat;
	//woodMat.ambientColor = glm::vec3(0.3f, 0.2f, 0.1f);  // brown ambient
	//woodMat.ambientStrength = 0.1f;
	woodMat.diffuseColor = glm::vec3(0.3f, 0.2f, 0.1f);  // brown diffuse
	woodMat.specularColor = glm::vec3(0.01f, 0.01f, 0.01f); // low specular
	woodMat.shininess = 4.0f;                           // low shininess
	woodMat.tag = "wood";
	m_objectMaterials.push_back(woodMat);
}

/***********************************************************
 *  SetupSceneLights()
 *
 *  This method is called to add and configure the light
 *  sources for the 3D scene.  There are up to 4 light sources.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// this line of code is NEEDED for telling the shaders to render 
	// the 3D scene with custom lighting, if no light sources have
	// been added then the display window will be black - to use the 
	// default OpenGL lighting then comment out the following line
	m_pShaderManager->setBoolValue(g_UseLightingName, true);

	/*** STUDENTS - add the code BELOW for setting up light sources ***/
	/*** Up to four light sources can be defined. Refer to the code ***/
	/*** in the OpenGL Sample for help                              ***/

	// Light 0 - pink from front right
	m_pShaderManager->setVec3Value("lightSources[0].position", 5.0f, 8.0f, 8.0f);
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 0.2f, 0.1f, 0.1f);
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 3.0f, 2.0f, 0.6f);
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", 0.9f, 0.9f, 0.9f);
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.5f);

	// Light 1 - softer pink, from front-left
	m_pShaderManager->setVec3Value("lightSources[1].position", -4.0f, 6.0f, 7.0f);
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.15f, 0.1f, 0.1f);
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.9f, 0.9f, 0.9f);
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.5f, 0.4f, 0.4f);
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.3f);

	// Light 2 - back grey light
	m_pShaderManager->setVec3Value("lightSources[2].position", 0.0f, 5.0f, -5.0f);
	m_pShaderManager->setVec3Value("lightSources[2].ambientColor", 0.1f, 0.1f, 0.2f);
	m_pShaderManager->setVec3Value("lightSources[2].diffuseColor", 0.6f, 0.45f, 1.2f);
	m_pShaderManager->setVec3Value("lightSources[2].specularColor", 0.3f, 0.2f, 0.5f);
	m_pShaderManager->setFloatValue("lightSources[2].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[2].specularIntensity", 0.4f);


	// Light 3 - top down neutral light to make sure everything has some light
	m_pShaderManager->setVec3Value("lightSources[3].position", 0.0f, 12.0f, 2.0f);
	m_pShaderManager->setVec3Value("lightSources[3].ambientColor", 0.1f, 0.1f, 0.15f);
	m_pShaderManager->setVec3Value("lightSources[3].diffuseColor", 0.9f, 0.6f, 0.9f);
	m_pShaderManager->setVec3Value("lightSources[3].specularColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setFloatValue("lightSources[3].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[3].specularIntensity", 0.2f);

}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// define the materials for objects in the scene
	DefineObjectMaterials();
	// add and define the light sources for the scene
	SetupSceneLights();

	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene - the following code loads
	// the basic 3D meshes into the graphics pipeline buffers

	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadConeMesh();
	m_basicMeshes->LoadSphereMesh();
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
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set the active color values in the shader (RGBA)
	//SetShaderColor(1, 1, 1, 1);

	// set shader to default pink material
	SetShaderMaterial("default");

	// draw the mesh with transformation values - this plane is used for the base
	m_basicMeshes->DrawPlaneMesh();
	/****************************************************************/

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.9f, 2.8f, 0.9f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 0.9f, 0.4f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set the active color values in the shader (RGBA)
	//SetShaderColor(1, 1, 1, 1);

	// set shader to wood material
	SetShaderMaterial("wood");

	m_basicMeshes->DrawCylinderMesh();
	/****************************************************************/

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 9.0f, 1.3f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 95.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.2f, 2.27f, 2.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set the active color values in the shader (RGBA)
	//SetShaderColor(1, 1, 1, 1);

	// set shader to wood material
	SetShaderMaterial("wood");

	m_basicMeshes->DrawBoxMesh();
	/****************************************************************/

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.7f, 1.5f, 1.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 40.0f;
	ZrotationDegrees = 8.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(3.3f, 3.85f, 2.19f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set the active color values in the shader (RGBA)
	//SetShaderColor(1, 1, 1, 1);

	// set shader to shiny material
	SetShaderMaterial("shiny");

	m_basicMeshes->DrawBoxMesh();
	/****************************************************************/

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 1.0f, 1.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(3.2f, 5.6f, 2.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set the active color values in the shader (RGBA)
	//SetShaderColor(1, 1, 1, 1);

	// set shader to shiny material
	SetShaderMaterial("shiny");

	m_basicMeshes->DrawSphereMesh();
	/****************************************************************/

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.2f, 4.0f, 1.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 5.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-3.3f, 2.50f, 2.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set the active color values in the shader (RGBA)
	//SetShaderColor(1, 1, 1, 1);

	// set shader to plastic material
	SetShaderMaterial("plastic");

	m_basicMeshes->DrawConeMesh();
	/****************************************************************/
}