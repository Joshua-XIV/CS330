#include "Centerpiece.h"
#include <glm/gtx/transform.hpp>

/***********************************************************
 *  Centerpiece()
 *
 *  Constructor - passes shader manager and meshes up to
 *  the SceneObject base class.
 ***********************************************************/
Centerpiece::Centerpiece(ShaderManager* shaderManager, ShapeMeshes* meshes) : SceneObject(shaderManager, meshes) {}

/***********************************************************
 *  Render()
 *
 *  Draws all parts of the crystal prism centerpiece relative
 *  to the given position.
 *
 *  Multiple parts are offset from a given position to create
 *  the entire object as one unit. Rotations are applied and
 *  rotated from that relative position.
 ***********************************************************/
void Centerpiece::Render(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation) {
    glm::mat4 rotation = BuildRotationMatrix(xRotation, yRotation, zRotation);

    // glass-like material - high specular, low diffuse, cool blue tint
    m_pShaderManager->setIntValue("bUseTexture", false);
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(1.0f, 1.0f, 1.0f));

    // --- flat base --- thin dark cylinder the prism rests on
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.15f, 0.15f, 0.18f));
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.15f, 0.15f, 0.18f, 1.0f));

    // no offset, base sits at position
    SetTransformations(glm::vec3(0.35f * scale, 0.04f * scale, 0.35f * scale),
        xRotation, yRotation, zRotation, position);
    m_basicMeshes->DrawCylinderMesh(true, true, true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // --- prism body --- crystal glass triangular prism standing upright
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.05f, 0.08f, 0.1f));
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.5f, 0.5f, 0.5f));
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.4f, 0.55f, 0.6f, 0.3f));

    // offset up so it sits on top of the base
    glm::vec3 prismOffset = ScaledOffset(rotation, scale, 0.0f, 0.75f, 0.08f);
    SetTransformations(glm::vec3(0.56f * scale, 1.4f * scale, 0.56f * scale),
        xRotation, yRotation, zRotation, position + prismOffset);
    m_basicMeshes->DrawPrismMesh();

    // --- inner highlight --- slightly smaller prism inside main body
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.02f, 0.04f, 0.05f));
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.5f, 0.5f, 0.5f));
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.5f, 0.7f, 0.75f, 0.2f));

    // same offset as prism body, slightly smaller scale
    SetTransformations(glm::vec3(0.36f * scale, 1.36f * scale, 0.36f * scale),
        xRotation, yRotation, zRotation, position + prismOffset);
    m_basicMeshes->DrawPrismMesh();
    glDisable(GL_BLEND);
}