#pragma once
#include "SceneObject.h"

/***********************************************************
 *  Laptop
 *
 *  This class handles the rendering of a 3D laptop object
 *  composed of multiple meshes.
 *  All parts are rendered relative to the given position
 *  so the entire mug moves and rotates as one unit.
 ***********************************************************/
class Laptop : SceneObject {
public:
	// constructor
	Laptop(ShaderManager* shaderManager, ShapeMeshes* meshes, int laptopFrameTexture = 0, int keyTexture = 0);

	// Draws the laptop at the given world position.
	// All parts are positioned relative to the given position.
	//
	// position   - world space position of the mug base
	// scale      - scale applied to all parts
	// xRotation  - rotation in degrees around the X axis
	// yRotation  - rotation in degrees around the Y axis
	// zRotation  - rotation in degrees around the Z axis
	void Render(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		float scale = 1.0f,
		float xRotation = 0.0f,
		float yRotation = 0.0f,
		float zRotation = 0.0f) override;

	void SetLaptopFrameTexture(int laptopFrameTexture) { m_laptopFrameTexture = laptopFrameTexture; };

	void SetKeyTexture(int keyTexture) { m_keyTexture = keyTexture; };

private:
	int m_laptopFrameTexture = 0;
	int m_keyTexture = 0;
	// Helper function to render the keyboard to the laptop
	void RenderKeyboard(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation);
};