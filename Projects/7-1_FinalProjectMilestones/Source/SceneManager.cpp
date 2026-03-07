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

	//texture slot 12
	bReturn = CreateGLTexture(
		"textures/raw_cotton.jpg",
		"raw_cotton"
	);

	//texture slot 13
	bReturn = CreateGLTexture(
		"textire/branch.jpg",
		"branch"
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
 *  SetupSceneLights()
 *
 *  This method is called to add and configure the light
 *  sources for the 3D scene.  There are up to 4 light sources.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// this line of code is NEEDED for telling the shaders to render 
	// the 3D scene with custom lighting - to use the default rendered 
	// lighting then comment out the following line
	m_pShaderManager->setBoolValue("bUseLighting", true);

	// directional light - simulates light coming from above
	m_pShaderManager->setBoolValue("directionalLight.bActive", true);
	m_pShaderManager->setVec3Value("directionalLight.direction", -0.2f, -1.0f, -0.3f);
	m_pShaderManager->setVec3Value("directionalLight.ambient", 0.15f, 0.15f, 0.15f);
	m_pShaderManager->setVec3Value("directionalLight.diffuse", 0.6f, 0.6f, 0.6f);
	m_pShaderManager->setVec3Value("directionalLight.specular", 0.4f, 0.4f, 0.4f);

	// point light - positioned above and in front of the scene for direct illumination
	m_pShaderManager->setBoolValue("pointLights[0].bActive", true);
	m_pShaderManager->setVec3Value("pointLights[0].position", 0.0f, 10.0f, 5.0f);
	m_pShaderManager->setVec3Value("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	m_pShaderManager->setVec3Value("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	m_pShaderManager->setVec3Value("pointLights[0].specular", 0.5f, 0.5f, 0.5f);

	// spotlight - centered directly above the table, wide cone to evenly light all objects
	m_pShaderManager->setBoolValue("spotLight.bActive", true);
	m_pShaderManager->setVec3Value("spotLight.position", 0.0f, 9.0f, 0.0f);
	m_pShaderManager->setVec3Value("spotLight.direction", 0.0f, -1.0f, 0.0f);
	m_pShaderManager->setVec3Value("spotLight.ambient", 0.8f, 0.8f, 0.8f);
	m_pShaderManager->setVec3Value("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_pShaderManager->setVec3Value("spotLight.specular", 1.0f, 1.0f, 1.0f);
	m_pShaderManager->setFloatValue("spotLight.constant", 1.0f);
	m_pShaderManager->setFloatValue("spotLight.linear", 0.09f);
	m_pShaderManager->setFloatValue("spotLight.quadratic", 0.032f);
	// wide cutoff angles keep the full table surface evenly lit
	m_pShaderManager->setFloatValue("spotLight.cutOff", glm::cos(glm::radians(60.f)));
	m_pShaderManager->setFloatValue("spotLight.outerCutOff", glm::cos(glm::radians(120.0f)));
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
	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene
	LoadSceneTextures();
	// add and define the light sources for the scene
	SetupSceneLights();
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadTorusMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadPrismMesh();
	m_basicMeshes->LoadSphereMesh();
	m_mug = new Mug(m_pShaderManager, m_basicMeshes);
	m_coaster = new Coaster(m_pShaderManager, m_basicMeshes, FindTextureSlot("coaster"));
	m_table = new Table(m_pShaderManager, m_basicMeshes, FindTextureSlot("table_leg"), FindTextureSlot("table"));
	m_book = new Book(m_pShaderManager, m_basicMeshes);
	m_laptop = new Laptop(m_pShaderManager, m_basicMeshes, FindTextureSlot("steel"), FindTextureSlot("black_plastic"));
	m_centerPiece = new Centerpiece(m_pShaderManager, m_basicMeshes, FindTextureSlot("branch"), FindTextureSlot("raw_cotton"));
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// set a default base color before rendering individual objects
	SetShaderColor(0.8f, 0.6f, 0.4f, 1.0f);

	// render the table centered at the world origin
	m_table->Render();

	// render the vase above the table's center
	m_centerPiece->Render(glm::vec3(0.0f, 5.24f, 0.0f));

	// render the mug on top of the table surface, rotated 165 degrees on Y
	m_mug->Render(glm::vec3(1.4f, 5.4f, 2.8f), 0.8f, 0.0f, 165.0f);

	// render the coaster directly beneath the mug at the table surface height
	m_coaster->Render(glm::vec3(1.4f, 5.24f, 2.8f), 1.12f);

	// render the laptop on top of the table's surface and placemat, rotation -25 degress on Y
	m_laptop->Render(glm::vec3(-1.9f, 5.32f, 3.75f), 0.95f, 0.0f, -25.0f, 0.0f);

	// set book cover and page textures then render on the table
	m_book->SetPageTexture(FindTextureSlot("pages"));
	m_book->SetCoverTexture(FindTextureSlot("brown_leather"));
	m_book->Render(glm::vec3(1.3f, 5.46f, -1.5f), 0.8f, 0.0f, 60.0f, 0.0f);

	// set book cover texture then render above the first book
	m_book->SetCoverTexture(FindTextureSlot("black_leather"));	// 7 - black leather
	m_book->Render(glm::vec3(1.3f, 5.87f, -1.5f), 0.65f, 0.0f, 240.0f, 0.0f);

	// set book cover texture and adjust uvscale then render above the second book
	m_book->SetCoverTexture(FindTextureSlot("red_leather")); // 8 - red leather
	m_book->SetUVScale(0.3f, 0.5f);
	m_book->Render(glm::vec3(1.3f, 6.19f, -1.5f), 0.5f, 0.0f, 60.0f, 0.0f);

	// render the wooden floor beneath the table and carpet
	RenderFloor();

	// render the carpet beneath the table
	RenderCarpet();

	// render three place mats at different positions on the table surface
	RenderPlaceMat(glm::vec3(-1.8f, 5.24f, 3.5f), 1.9f);
	RenderPlaceMat(glm::vec3(-2.5f, 5.24f, -1.5f), 1.9f);
	RenderPlaceMat(glm::vec3(3.7f, 5.24f, 0.7f), 1.9f);
}

/***********************************************************
 *  RenderFloor()
 *
 *  Renders the floor of the 3D scene as a large flat plane
 *  mesh centered at the world origin with a dark wood texture
 *  applied. The plane is scaled to extend beyond the carpet
 *  and table area to fill the visible scene background.
 ***********************************************************/
void SceneManager::RenderFloor()
{
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// scale to cover the full scene background area
	scaleXYZ = glm::vec3(15.0f, 0.01f, 15.0f);

	// no rotation needed - the plane lies flat by default
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	// apply scale, rotation, and position to the transform method
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// apply the dark wood texture to the floor surface
	SetShaderTexture("dark_wood");
	// tile the texture 1.5x in both directions to avoid a single stretched
	// image across the large plane - tiling creates a more realistic wood floor
	SetTextureUVScale(1.5f, 1.5f);

	// draw the plane mesh as the floor
	m_basicMeshes->DrawPlaneMesh();
}

/***********************************************************
 *  RenderCarpet()
 *
 *  Renders the carpet of the 3D scene as a large flat cylinder
 *  mesh with a carpet texture applied. The carpet is centered
 *  at the origin and scaled to cover the table area.
 ***********************************************************/
void SceneManager::RenderCarpet()
{
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// scale to cover the full table area, near-zero Y keeps the mesh flat
	scaleXYZ = glm::vec3(10.0f, 0.01f, 10.0f);

	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// no rotation needed - the carpet lies flat by default
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	// apply scale, rotation, and position to the transform method
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// apply the carpet texture to the floor surface
	SetShaderTexture("carpet");

	// tile the texture 1.5x in both directions so the carpet fiber pattern
	// repeats naturally rather than one image being stretched across the surface
	SetTextureUVScale(1.5f, 1.5f);

	// draw the flat cylinder mesh as the floor
	m_basicMeshes->DrawCylinderMesh();
}

/***********************************************************
 *  RenderPlaceMat()
 *
 *  Renders a circular place mat at the specified position
 *  using a flat cylinder mesh with a fabric texture applied.
 *
 *  position  - the XYZ world position of the place mat
 *  scale     - uniform scale factor for the mat's radius
 *  xRotation - rotation in degrees around the X axis (default 0)
 *  yRotation - rotation in degrees around the Y axis (default 0)
 *  zRotation - rotation in degrees around the Z axis (default 0)
 ***********************************************************/
void SceneManager::RenderPlaceMat(glm::vec3 position, float scale,
	float xRotation, float yRotation, float zRotation)
{
	// apply scale uniformly to X and Z to size the mat radius; near-zero Y keeps it flat
	SetTransformations(
		glm::vec3(1.0f * scale, 0.01f, 1.0f * scale),
		xRotation,
		yRotation,
		zRotation,
		position);

	// apply the fabric texture to give the mat a woven surface appearance
	SetShaderTexture("mat_fabric");

	// compress UV on the X axis to better match place mat proportions
	SetTextureUVScale(0.5f, 1.0f);

	// draw the flat cylinder mesh as the place mat
	m_basicMeshes->DrawCylinderMesh();
}

