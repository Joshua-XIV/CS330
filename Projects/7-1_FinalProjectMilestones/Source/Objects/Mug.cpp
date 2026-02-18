#include "Mug.h"
#include <glm/gtx/transform.hpp>

/***********************************************************
 *  Mug()
 *
 *  Constructor - passes shader manager and meshes up to
 *  the SceneObject base class.
 ***********************************************************/
Mug::Mug(ShaderManager* shaderManager, ShapeMeshes* meshes) : SceneObject(shaderManager, meshes) {}

/***********************************************************
 *  Render()
 *
 *  Draws all parts of the mug all relative to the given position.
 *
 *  Multiple parts are offset from a given position to create the
 *  entire object as one unit. This also means rotations were applied
 *  and rotated from that relative position.
 ***********************************************************/
void Mug::Render(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation) {
    // build rotation matrix for transforming offsets and the handle
    glm::mat4 rotation = BuildRotationMatrix(xRotation, yRotation, zRotation);

    // no texture, mug uses flat colors
    m_pShaderManager->setIntValue("bUseTexture", false);

    // --- base ring --- brown band at the bottom
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.545f, 0.271f, 0.075f));
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.545f, 0.271f, 0.075f));
    m_pShaderManager->setFloatValue("material.shininess", 4.0f);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.545f, 0.271f, 0.075f, 1.0f));

    // offset -0.01 down to sit flush at the base
    glm::vec3 outerBottomOffset = glm::vec3(rotation * glm::vec4(0.0f, -0.01f * scale, 0.0f, 0.0f));
    SetTransformations(glm::vec3(0.61f * scale, 0.06f * scale, 0.61f * scale),
        xRotation, yRotation, zRotation, position + outerBottomOffset);
    m_basicMeshes->DrawCylinderMesh(false, true, true);

    // --- white band --- decorative stripe
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

    // offset 0.05 up from base
    glm::vec3 middleBottomOffset = glm::vec3(rotation * glm::vec4(0.0f, 0.05f * scale, 0.0f, 0.0f));
    SetTransformations(glm::vec3(0.61f * scale, 0.3f * scale, 0.61f * scale),
        xRotation, yRotation, zRotation, position + middleBottomOffset);
    m_basicMeshes->DrawCylinderMesh(false, false, true);

    // --- top ring --- brown band matching base
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.545f, 0.271f, 0.075f, 1.0f));

    // offset 0.35 up
    glm::vec3 topBottomOffset = glm::vec3(rotation * glm::vec4(0.0f, 0.35f * scale, 0.0f, 0.0f));
    SetTransformations(glm::vec3(0.61f * scale, 0.05f * scale, 0.61f * scale),
        xRotation, yRotation, zRotation, position + topBottomOffset);
    m_basicMeshes->DrawCylinderMesh(false, false, true);

    // --- outer body --- main teal cylinder drawn over the bands, no offset
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.4f, 0.55f, 0.5f));
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.05f, 0.05f, 0.05f));
    m_pShaderManager->setFloatValue("material.shininess", 4.0f);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.4f, 0.55f, 0.5f, 1.0f));

    SetTransformations(glm::vec3(0.6f * scale, 1.2f * scale, 0.6f * scale),
        xRotation, yRotation, zRotation, position);
    m_basicMeshes->DrawCylinderMesh(false, true, true);

    // --- inner wall --- slightly smaller radius to create hollow look
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.35f, 0.5f, 0.45f, 1.0f));

    // offset 0.05 up so it sits inside the rim
    glm::vec3 innerOffset = glm::vec3(rotation * glm::vec4(0.0f, 0.05f * scale, 0.0f, 0.0f));
    SetTransformations(glm::vec3(0.55f * scale, 1.15f * scale, 0.55f * scale),
        xRotation, yRotation, zRotation, position + innerOffset);
    m_basicMeshes->DrawCylinderMesh(false, true, true);

    // --- handle --- half torus, base rotation 270 on Z orients it upright
    // mug rotation matrix applied on top to follow object rotation
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.4f, 0.55f, 0.5f, 1.0f));
    glm::mat4 handleBaseRot = glm::rotate(glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 handleRotation = rotation * handleBaseRot;

    // offset 0.59 on X and 0.8 on Y to sit on the side of the mug
    glm::vec3 handleOffset = glm::vec3(rotation * glm::vec4(0.59f * scale, 0.8f * scale, 0.0f, 0.0f));
    SetTransformations(glm::vec3(0.3f * scale, 0.3f * scale, 0.8f * scale),
        handleRotation, position + handleOffset);
    m_basicMeshes->DrawHalfTorusMesh();
}