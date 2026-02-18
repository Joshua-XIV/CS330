#include "Coaster.h"
#include <glm/gtx/transform.hpp>

/***********************************************************
 *  Coaster()
 *
 *  Constructor - passes shader manager and meshes up to
 *  the SceneObject base class.
 ***********************************************************/
Coaster::Coaster(ShaderManager* shadeManager, ShapeMeshes* meshes) : SceneObject(shadeManager, meshes) {}

/***********************************************************
 *  Render()
 *
 *  Draws all parts of the coaster all relative to the given position.
 *
 *  Multiple parts are offset from a given position to create the
 *  entire object as one unit. This also means rotations were applied
 *  and rotated from that relative position.
 ***********************************************************/
void Coaster::Render(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation) {
	glm::mat4 rotation = BuildRotationMatrix(xRotation, yRotation, zRotation);

	// enable textures for coaster
	m_pShaderManager->setIntValue("bUseTexture", true);
    m_pShaderManager->setSampler2DValue("objectTexture", 0); // coaster_wood is at slot 0

    // --- flat base  sides only ---
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(8.0f, 0.3f));
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.7f, 0.65f, 0.6f));
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.02f, 0.02f, 0.02f));
    m_pShaderManager->setFloatValue("material.shininess", 2.0f);
    SetTransformations(glm::vec3(0.7f * scale, 0.1f * scale, 0.7f * scale),
        xRotation, yRotation, zRotation, position);
    m_basicMeshes->DrawCylinderMesh(false, false, true);

    // -- flat base top and bottom only
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(1.0f, 1.0f));
    SetTransformations(glm::vec3(0.7f * scale, 0.1f * scale, 0.7f * scale),
        xRotation, yRotation, zRotation, position);
    m_basicMeshes->DrawCylinderMesh(true, true, false);

    // --- ring around base ---
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(3.0f, 0.7f));
    glm::mat4 ringBaseRot = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 ringRotation = rotation * ringBaseRot;
    glm::vec3 ringOffset = glm::vec3(rotation * glm::vec4(0.0f, 0.1f * scale, 0.0f, 0.0f));
    SetTransformations(glm::vec3(0.58f * scale, 0.58f * scale, 0.35f * scale),
        ringRotation, position + ringOffset);
    m_basicMeshes->DrawTorusMesh();
}