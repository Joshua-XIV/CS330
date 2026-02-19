#include "Book.h"
#include <glm/gtx/transform.hpp>

/***********************************************************
 *  Book()
 *
 *  Constructor - passes shader manager and meshes up to
 *  the SceneObject base class.
 ***********************************************************/
Book::Book(ShaderManager* shaderManager, ShapeMeshes* meshes, int coverTextureSlot = 0) 
    : SceneObject(shaderManager, meshes), m_coverTextureSlot(coverTextureSlot) {}

/***********************************************************
 *  Render()
 *
 *  Draws all parts of the book relative to the given position.
 *  Cover texture slot is passed in so multiple books can use
 *  different cover textures.
 ***********************************************************/
void Book::Render(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation) {
    glm::mat4 rotation = BuildRotationMatrix(xRotation, yRotation, zRotation);

    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.05f, 0.05f, 0.05f));
    m_pShaderManager->setFloatValue("material.shininess", 4.0f);

    // --- front cover --- uses passed in cover texture slot
    m_pShaderManager->setIntValue("bUseTexture", true);
    m_pShaderManager->setSampler2DValue("objectTexture", m_coverTextureSlot);
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(1.0f, 1.0f));

    // offset 0.25 up to sit on top of the pages
    glm::vec3 frontCoverOffset = ScaledOffset(rotation, scale, 0.0f, 0.25f, 0.0f);
    SetTransformations(glm::vec3(2.0f * scale, 0.05f * scale, 3.0f * scale),
        xRotation, yRotation, zRotation, position + frontCoverOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- back cover --- same texture as front cover
    // offset -0.25 down to sit below the pages
    glm::vec3 backCoverOffset = ScaledOffset(rotation, scale, 0.0f, -0.25f, 0.0f);
    SetTransformations(glm::vec3(2.0f * scale, 0.05f * scale, 3.0f * scale),
        xRotation, yRotation, zRotation, position + backCoverOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- pages --- flat color, slightly smaller than covers
    m_pShaderManager->setIntValue("bUseTexture", false);
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.95f, 0.92f, 0.85f));
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.95f, 0.92f, 0.85f, 1.0f));

    // no offset, centered between the two covers
    SetTransformations(glm::vec3(1.9f * scale, 0.45f * scale, 2.9f * scale),
        xRotation, yRotation, zRotation, position);
    m_basicMeshes->DrawBoxMesh();

    // --- spine --- thin box on the left side connecting covers
    m_pShaderManager->setIntValue("bUseTexture", true);
    m_pShaderManager->setSampler2DValue("objectTexture", m_coverTextureSlot);
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(1.0f, 1.0f));

    // offset -1.0 on X to sit on the left edge
    glm::vec3 spineOffset = ScaledOffset(rotation, scale, -1.0f, 0.0f, 0.0f);
    SetTransformations(glm::vec3(0.05f * scale, 0.55f * scale, 3.0f * scale),
        xRotation, yRotation, zRotation, position + spineOffset);
    m_basicMeshes->DrawBoxMesh();
}