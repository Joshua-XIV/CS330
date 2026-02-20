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
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
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

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
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
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
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

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/***********************************************************
 *  LoadSceneTextures()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::LoadSceneTextures()
{
	bool bReturn = false;

	// texture slot 0
	bReturn = CreateGLTexture(
		"textures/coaster_wood.jpg",
		"coaster");

	// texture slot 1
	bReturn = CreateGLTexture(
		"textures/table_wood.jpg",
		"table");

	// texture slot 2
	bReturn = CreateGLTexture(
		"textures/table_leg_wood.jpg",
		"table_leg"
	);

	// texture slot 3
	bReturn = CreateGLTexture(
		"textures/carpet.jpg",
		"carpet"
	);

	// texture slot 4
	bReturn = CreateGLTexture(
		"textures/mat_fabric.jpg",
		"mat_fabric"
	);

	// texture slot 5
	bReturn = CreateGLTexture(
		"textures/dark_wood.jpg",
		"dark_wood"
	);

	// texture slot 6
	bReturn = CreateGLTexture(
		"textures/brown_leather.jpg",
		"brown_leather"
	);

	// texture slot 7
	bReturn = CreateGLTexture(
		"textures/black_leather.jpg",
		"black_leather"
	);

	// texture slot 8
	bReturn = CreateGLTexture(
		"textures/red_leather.jpg",
		"red_leather"
	);

	//texture slot 9
	bReturn = CreateGLTexture(
		"textures/pages.jpg",
		"pages"
	);

	//texture slot 10
	bReturn = CreateGLTexture(
		"textures/steel.jpg",
		"steel"
	);

	//texture slot 11
	bReturn = CreateGLTexture(
		"textures/black_plastic.jpg",
		"black_plastic"
	);

	// after the texture image data is loaded into memory, the
	// loaded textures need to be bound to texture slots - there
	// are a total of 16 available slots for scene textures
	BindGLTextures();
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
	LoadSceneTextures();
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadTorusMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadPrismMesh();
	m_mug = new Mug(m_pShaderManager, m_basicMeshes);
	m_coaster = new Coaster(m_pShaderManager, m_basicMeshes, FindTextureSlot("coaster"));
	m_table = new Table(m_pShaderManager, m_basicMeshes, FindTextureSlot("table_leg"), FindTextureSlot("table"));
	m_book = new Book(m_pShaderManager, m_basicMeshes);
	m_laptop = new Laptop(m_pShaderManager, m_basicMeshes, FindTextureSlot("steel"), FindTextureSlot("black_plastic"));
	m_centerPiece = new Centerpiece(m_pShaderManager, m_basicMeshes);
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	SetShaderColor(0.8f, 0.6f, 0.4f, 1.0f);
	m_table->Render();
	m_centerPiece->Render(glm::vec3(0.0f, 5.24f, 0.0f));
	m_mug->Render(glm::vec3(1.4f, 5.4f, 2.8f), 0.8f, 0.0f, 165.0f);

	// redner coaster and laptop
	m_coaster->Render(glm::vec3(1.4f, 5.24f, 2.8f), 1.12f);
	m_laptop->Render(glm::vec3(-1.9f, 5.32f, 3.75f), 0.95f, 0.0f, -25.0f, 0.0f);

	// set book cover and page textures then render
	m_book->SetPageTexture(FindTextureSlot("pages"));
	m_book->SetCoverTexture(FindTextureSlot("brown_leather"));
	m_book->Render(glm::vec3(1.3f, 5.46f, -1.5f), 0.8f, 0.0f, 60.0f, 0.0f);

	// set book cover texture then render
	m_book->SetCoverTexture(FindTextureSlot("black_leather"));	// 7 - black leather
	m_book->Render(glm::vec3(1.3f, 5.87f, -1.5f), 0.65f, 0.0f, 240.0f, 0.0f);

	// set book cover texture and adjust uvscale then render
	m_book->SetCoverTexture(FindTextureSlot("red_leather")); // 8 - red leather
	m_book->SetUVScale(0.3f, 0.5f);
	m_book->Render(glm::vec3(1.3f, 6.19f, -1.5f), 0.5f, 0.0f, 60.0f, 0.0f);

	// render wooden floor and grey carpet
	RenderFloor();
	RenderCarpet();

	// render placemats on table
	RenderPlaceMat(glm::vec3(-1.8f, 5.24f, 3.5f), 1.9f);
	RenderPlaceMat(glm::vec3(-2.5f, 5.24f, -1.5f), 1.9f);
	RenderPlaceMat(glm::vec3(3.7f, 5.24f, 0.7f), 1.9f);
}

/***********************************************************
 *  RenderFloor()
 *
 *  Renders the floor plane with a dark wood texture.
 ***********************************************************/
void SceneManager::RenderFloor()
{
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	scaleXYZ = glm::vec3(15.0f, 0.01f, 15.0f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("dark_wood");
	SetTextureUVScale(1.5f, 1.5f);

	m_basicMeshes->DrawPlaneMesh();
}

/***********************************************************
 *  RenderCarpet()
 *
 *  Renders a large cylinder to represent a carpet or rug
 *  sitting on top of the floor.
 ***********************************************************/
void SceneManager::RenderCarpet()
{
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	scaleXYZ = glm::vec3(10.0f, 0.01f, 10.0f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("carpet");
	SetTextureUVScale(1.5f, 1.5f);

	m_basicMeshes->DrawCylinderMesh();
}

/***********************************************************
 *  RenderPlaceMat()
 *
 *  Renders a circular place mat at the given position.
 *  Multiple place mats can be rendered by calling this
 *  method with different positions and scales.
 *
 *  position   - world space position of the place mat
 *  scale      - uniform scale applied to the place mat
 *  xRotation  - rotation in degrees around the X axis
 *  yRotation  - rotation in degrees around the Y axis
 *  zRotation  - rotation in degrees around the Z axis
 ***********************************************************/
void SceneManager::RenderPlaceMat(glm::vec3 position, float scale,
	float xRotation, float yRotation, float zRotation)
{
	SetTransformations(
		glm::vec3(1.0f * scale, 0.01f, 1.0f * scale),
		xRotation,
		yRotation,
		zRotation,
		position);

	SetShaderTexture("mat_fabric");
	SetTextureUVScale(0.5f, 1.0f);

	m_basicMeshes->DrawCylinderMesh();
}
