#pragma once
#include "SceneObject.h"

/***********************************************************
 *  Table
 *
 *  Renders a 3D table object composed of a flat top surface
 *  and multiple legs. All parts are positioned relative to
 *  the given position so the entire table moves as one unit.
 ***********************************************************/
class Table : public SceneObject {
public:
	// constructor
	Table(ShaderManager* shadeManager, ShapeMeshes* basicMeshes, int woodLegTexture = 0, int tableTopTexture = 0);

	// Draws the table at the given world position.
	// All parts are positioned relative to the given position.
	//
	// position   - world space position of the table base
	// scale      - uniform scale applied to all parts
	// xRotation  - rotation in degrees around the X axis
	// yRotation  - rotation in degrees around the Y axis
	// zRotation  - rotation in degrees around the Z axis
	void Render(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		float scale = 1.0f,
		float xRotation = 0.0f,
		float yRotation = 0.0f,
		float zRotation = 0.0f) override;

	void SetWoodTexture(int woodLegTexture) { m_woodLegTexture = woodLegTexture; };

	void SetTableTopTexture(int tableTopTexture) { m_tableTopTexture = tableTopTexture;};

private:
	int m_woodLegTexture = 0;
	int m_tableTopTexture = 0;
};