#include "Mug.h"
#include <glm/gtx/transform.hpp>

/***********************************************************
 *  Mug()
 *
 *  Constructor - stores references to the shader manager
 *  and shape meshes needed for rendering.
 ***********************************************************/
Mug::Mug(ShaderManager* shaderManager, ShapeMeshes* meshes)
    : m_pShaderManager(shaderManager), m_basicMeshes(meshes) {
}

/***********************************************************
 *  SetTransformations()
 *
 *  Builds and uploads the model matrix from X, Y, Z
 *  rotation values. Rotation order is Z * Y * X.
 *  Similar to SceneManager's Set Transformations
 ***********************************************************/
void Mug::SetTransformations(glm::vec3 scaleXYZ, float rotX, float rotY, float rotZ, glm::vec3 positionXYZ)
{
    glm::mat4 scale = glm::scale(scaleXYZ);
    glm::mat4 rotationX = glm::rotate(glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationY = glm::rotate(glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationZ = glm::rotate(glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 translation = glm::translate(positionXYZ);
    glm::mat4 modelView = translation * rotationZ * rotationY * rotationX * scale;
    m_pShaderManager->setMat4Value("model", modelView);
}

/***********************************************************
 *  SetTransformations()
 *
 *  Overload that accepts a precomputed rotation matrix.
 *  Used for the handle to avoid rotation conversion issues.
 ***********************************************************/
void Mug::SetTransformations(glm::vec3 scaleXYZ, glm::mat4 rotation, glm::vec3 positionXYZ)
{
    glm::mat4 scale = glm::scale(scaleXYZ);
    glm::mat4 translation = glm::translate(positionXYZ);
    glm::mat4 modelView = translation * rotation * scale;
    m_pShaderManager->setMat4Value("model", modelView);
}

/***********************************************************
 *  Render()
 *
 *  Draws the three parts of the mug - outer body, inner
 *  wall, and handle - all relative to the given position.
 *
 *  The handle offset is transformed by the mug's rotation
 *  matrix so it stays attached to the correct side
 *  regardless of how the mug is rotated.
 ***********************************************************/
void Mug::Render(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation) {
    // build rotation matrix for transforming offsets and the handle
    glm::mat4 rotX = glm::rotate(glm::radians(xRotation), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotY = glm::rotate(glm::radians(yRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotZ = glm::rotate(glm::radians(zRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 rotation = rotZ * rotY * rotX;

    // disable textures
    m_pShaderManager->setIntValue("bUseTexture", false);

    // outer body - full cylinder with bottom, no top
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.4f, 0.55f, 0.5f));
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.05f, 0.05f, 0.05f));
    m_pShaderManager->setFloatValue("material.shininess", 4.0f);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.4f, 0.55f, 0.5f, 1.0f));

    // outer body transformations
    SetTransformations(glm::vec3(0.6f * scale, 1.2f * scale, 0.6f * scale),
        xRotation, yRotation, zRotation, position);
    m_basicMeshes->DrawCylinderMesh(false, true, true);

    // inner wall - slightly smaller, offset upward to sit inside the rim
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.4f, 0.55f, 0.5f));
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.05f, 0.05f, 0.05f));
    m_pShaderManager->setFloatValue("material.shininess", 4.0f);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.35f, 0.5f, 0.45f, 1.0f));

    // inner body transformations
    glm::vec3 innerOffset = glm::vec3(rotation * glm::vec4(0.0f, 0.05f * scale, 0.0f, 0.0f));
    SetTransformations(glm::vec3(0.55f * scale, 1.15f * scale, 0.55f * scale),
        xRotation, yRotation, zRotation, position + innerOffset);
    m_basicMeshes->DrawCylinderMesh(false, true, true);

    // handle - half torus rotated to stand upright on the side of the mug
    // base rotation of 270 on Z orients the torus correctly at zero rotation
    // mug rotation matrix is applied on top to keep it attached at all angles
    glm::mat4 handleBaseRot = glm::rotate(glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 handleRotation = rotation * handleBaseRot;
    glm::vec3 handleOffset = glm::vec3(rotation * glm::vec4(0.59f * scale, 0.7f * scale, 0.0f, 0.0f));

    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.4f, 0.55f, 0.5f));
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.05f, 0.05f, 0.05f));
    m_pShaderManager->setFloatValue("material.shininess", 4.0f);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.4f, 0.55f, 0.5f, 1.0f));

    // handle transformations
    SetTransformations(glm::vec3(0.35f * scale, 0.3f * scale, 0.8f * scale),
        handleRotation, position + handleOffset);
    m_basicMeshes->DrawHalfTorusMesh();
}