#include "Table.h"
#include <glm/gtx/transform.hpp>

Table::Table(ShaderManager* shaderManager, ShapeMeshes* basicMeshes) : SceneObject(shaderManager, basicMeshes) {}

void Table::Render(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation) {
	glm::mat4 rotation = BuildRotationMatrix(xRotation, yRotation, zRotation);

	// enable table wood texture
	m_pShaderManager->setIntValue("bUseTexture", true);
	m_pShaderManager->setSampler2DValue("objectTexture", 2);
	m_pShaderManager->setVec2Value("UVscale", glm::vec2(1.0f, 1.0f));

	float SLANT_LEG_Y_SCALE = 2.5f;
	float SLANT_LEG_X_OFFSET = 0.7f;
	float BOTTOM_SLANT_LEG_Y_OFFSET = 1.0f;
	float UPPER_SLANT_LEG_Y_OFFSET = 4.0f;
	float SLANT_LEG_Z_OFFSET = 0.7f;
	float SLANT_LEG_ANGLE = 60.0f;

	// --- bottom front left leg ---
	glm::mat4 frontLeftLegBaseRot = glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(-1.0f, 0.0f, -1.0f));
	glm::mat4 frontLeftLegRot = rotation * frontLeftLegBaseRot;

	glm::vec3 frontLeftLegOffset = glm::vec3(-SLANT_LEG_X_OFFSET, BOTTOM_SLANT_LEG_Y_OFFSET, SLANT_LEG_X_OFFSET);
	SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
		frontLeftLegRot, position + frontLeftLegOffset);
	m_basicMeshes->DrawBoxMesh();

	// --- bottom front right leg ---
	glm::mat4 frontRightLegBaseRot = glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(-1.0f, 0.0f, 1.0f));
	glm::mat4 frontRightLegRot = rotation * frontRightLegBaseRot;
	glm::vec3 frontRightLegOffset = glm::vec3(SLANT_LEG_X_OFFSET, BOTTOM_SLANT_LEG_Y_OFFSET, SLANT_LEG_X_OFFSET);
	SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
		frontRightLegRot, position + frontRightLegOffset);
	m_basicMeshes->DrawBoxMesh();

	// --- bottom back left leg ---
	glm::mat4 backLeftLegBaseRot = glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(1.0f, 0.0f, -1.0f));
	glm::mat4 backLeftLegRot = rotation * backLeftLegBaseRot;
	glm::vec3 backLeftLegOffset = glm::vec3(-SLANT_LEG_X_OFFSET, BOTTOM_SLANT_LEG_Y_OFFSET, -SLANT_LEG_X_OFFSET);
	SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
		backLeftLegRot, position + backLeftLegOffset);
	m_basicMeshes->DrawBoxMesh();

	// --- bottom back right leg ---
	glm::mat4 backRightLegBaseRot = glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(1.0f, 0.0f, 1.0f));
	glm::mat4 backRightLegRot = rotation * backRightLegBaseRot;
	glm::vec3 backRightLegOffset = glm::vec3(SLANT_LEG_X_OFFSET, BOTTOM_SLANT_LEG_Y_OFFSET, -SLANT_LEG_X_OFFSET);
	SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
		backRightLegRot, position + backRightLegOffset);
	m_basicMeshes->DrawBoxMesh();

	// --- middle leg leg ---
	glm::vec3 middleLegOffset = glm::vec3(0.0f, 2.5f, 0.0f);
	SetTransformations(glm::vec3(1.0f * scale, 3.0 * scale, 1.0f * scale),
		xRotation, yRotation, zRotation, position + middleLegOffset);
	m_basicMeshes->DrawBoxMesh();

	glm::mat4 yAxisFlip = glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	// --- upper front left leg ---
	glm::mat4 upperFrontLeftLegBaseRot = yAxisFlip * glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(1.0f, 0.0f, 1.0f));
	glm::mat4 upperFrontLeftLegRot = rotation * upperFrontLeftLegBaseRot;
	glm::vec3 upperFrontLeftLegOffset = glm::vec3(SLANT_LEG_X_OFFSET, UPPER_SLANT_LEG_Y_OFFSET, -SLANT_LEG_Z_OFFSET);
	SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
		upperFrontLeftLegRot, position + upperFrontLeftLegOffset);
	m_basicMeshes->DrawBoxMesh();

	// --- upper front right leg ---
	glm::mat4 upperFrontRightLegBaseRot = yAxisFlip * glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(1.0f, 0.0f, -1.0f));
	glm::mat4 upperFrontRightLegRot = rotation * upperFrontRightLegBaseRot;
	glm::vec3 upperFrontRightLegOffset = glm::vec3(-SLANT_LEG_X_OFFSET, UPPER_SLANT_LEG_Y_OFFSET, -SLANT_LEG_Z_OFFSET);
	SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
		upperFrontRightLegRot, position + upperFrontRightLegOffset);
	m_basicMeshes->DrawBoxMesh();

	// --- upper back left leg ---
	glm::mat4 upperBackLeftLegBaseRot = yAxisFlip * glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(-1.0f, 0.0f, 1.0f));
	glm::mat4 upperBackLeftLegRot = rotation * upperBackLeftLegBaseRot;
	glm::vec3 upperBackLeftLegOffset = glm::vec3(SLANT_LEG_X_OFFSET, UPPER_SLANT_LEG_Y_OFFSET, SLANT_LEG_Z_OFFSET);
	SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
		upperBackLeftLegRot, position + upperBackLeftLegOffset);
	m_basicMeshes->DrawBoxMesh();

	// --- upper back right leg ---
	glm::mat4 upperBackRightLegBaseRot = yAxisFlip * glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(-1.0f, 0.0f, -1.0f));
	glm::mat4 upperBackRightLegRot = rotation * upperBackRightLegBaseRot;
	glm::vec3 upperBackRightLegOffset = glm::vec3(-SLANT_LEG_X_OFFSET, UPPER_SLANT_LEG_Y_OFFSET, SLANT_LEG_Z_OFFSET);
	SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
		upperBackRightLegRot, position + upperBackRightLegOffset);
	m_basicMeshes->DrawBoxMesh();

	// --- table top ---
	m_pShaderManager->setSampler2DValue("objectTexture", 1);
	m_pShaderManager->setVec2Value("UVscale", glm::vec2(1.0f, 1.0f));
	glm::vec3 tapleTopOffset = glm::vec3(0.f, 5.03f, 0.0f);
	SetTransformations(glm::vec3(6.0f * scale, 0.2f * scale, 6.0f * scale),
		xRotation, yRotation, zRotation, position + tapleTopOffset);
	m_basicMeshes->DrawCylinderMesh();
}