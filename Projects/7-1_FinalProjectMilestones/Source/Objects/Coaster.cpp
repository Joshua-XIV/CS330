#include "Coaster.h"
#include <glm/gtx/transform.hpp>

/***********************************************************
 *  Coaster()
 *
 *  Constructor - passes shader manager and meshes up to
 *  the SceneObject base class.
 ***********************************************************/
Coaster::Coaster(ShaderManager* shadeManager, ShapeMeshes* meshes, int coasterTexture) 
    : SceneObject(shadeManager, meshes), m_coasterTexture(coasterTexture) {}

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
    // build rotation matrix for transforming offsets and the handle
	glm::mat4 rotation = BuildRotationMatrix(xRotation, yRotation, zRotation);

    m_pShaderManager->setIntValue("bUseTexture", true);
    m_pShaderManager->setSampler2DValue("objectTexture", m_coasterTexture);
    SetShaderMaterial(MAT_COASTER);

    // --- flat base sides --- tiled UV to avoid stretching on the thin sides, no offset
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(8.0f, 0.3f));
    SetTransformations(glm::vec3(0.7f * scale, 0.1f * scale, 0.7f * scale),
        xRotation, yRotation, zRotation, position);
    m_basicMeshes->DrawCylinderMesh(false, false, true);

    // --- flat base top and bottom --- normal UV for flat faces, no offset
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(1.0f, 1.0f));
    SetTransformations(glm::vec3(0.7f * scale, 0.1f * scale, 0.7f * scale),
        xRotation, yRotation, zRotation, position);
    m_basicMeshes->DrawCylinderMesh(true, true, false);

    // --- raised ring --- torus sitting on the edge of the base
    // base rotation 90 on X orients the torus flat
    // mug rotation matrix applied on top to follow object rotation
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(3.0f, 0.7f));
    glm::mat4 ringBaseRot = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 ringRotation = rotation * ringBaseRot;

    // offset 0.1 up to sit on top of the base cylinder
    glm::vec3 ringOffset = ScaledOffset(rotation, scale, 0.0f, 0.1f, 0.0f);
    SetTransformations(glm::vec3(0.58f * scale, 0.58f * scale, 0.35f * scale),
        ringRotation, position + ringOffset);
    m_basicMeshes->DrawTorusMesh();
}