#include "Laptop.h"
#include <glm/gtx/transform.hpp>

/***********************************************************
 *  Laptop()
 *
 *  Constructor - passes shader manager and meshes up to
 *  the SceneObject base class.
 ***********************************************************/
Laptop::Laptop(ShaderManager* shaderManager, ShapeMeshes* meshes) : SceneObject(shaderManager, meshes) {}

/***********************************************************
 *  Render()
 *
 *  Draws all parts of the laptop relative to the given position.
 *
 *  Multiple parts are offset from a given position to create the
 *  entire object as one unit. This also means rotations were applied
 *  and rotated from that relative position.
 ***********************************************************/
void Laptop::Render(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation) {
    glm::mat4 rotation = BuildRotationMatrix(xRotation, yRotation, zRotation);

    m_pShaderManager->setIntValue("bUseTexture", false);
    m_pShaderManager->setVec3Value("material.specularColor", glm::vec3(0.4f, 0.4f, 0.4f));
    m_pShaderManager->setFloatValue("material.shininess", 32.0f);

    // --- base / keyboard deck --- flat silver box, no offset
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.76f, 0.76f, 0.76f));
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.76f, 0.76f, 0.76f, 1.0f));

    SetTransformations(glm::vec3(3.0f * scale, 0.1f * scale, 2.0f * scale),
        xRotation, yRotation, zRotation, position);
    m_basicMeshes->DrawBoxMesh();

    // --- screen panel --- slightly thinner box, hinged open at the back
    // offset 1.0 back on Z and 1.0 up, tilted open on X
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.76f, 0.76f, 0.76f, 1.0f));
    glm::mat4 screenBaseRot = glm::rotate(glm::radians(-100.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 screenRotation = rotation * screenBaseRot;
    glm::vec3 screenOffset = ScaledOffset(rotation, scale, 0.0f, 0.79f, -1.08f);

    SetTransformations(glm::vec3(3.0f * scale, 0.08f * scale, 1.5f * scale),
        screenRotation, position + screenOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- screen outline --- dark outline around the screen
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.05f, 0.05f, 0.05f));
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));

    glm::vec3 screenOutlineOffset = ScaledOffset(rotation, scale, 0.0f, 0.8f, -1.05f);
    SetTransformations(glm::vec3(2.9f * scale, 0.03f * scale, 1.44f * scale),
        screenRotation, position + screenOutlineOffset);

    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::left);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::right);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::front);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::back);

    // --- screen face --- dark box on the inner face of the screen panel
    m_pShaderManager->setVec3Value("material.diffuseColor", glm::vec3(0.05f, 0.05f, 0.05f));
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));

    glm::vec3 screenFaceOffset = ScaledOffset(rotation, scale, 0.0f, 0.8f, -1.04f);
    SetTransformations(glm::vec3(2.8f * scale, 0.01f * scale, 1.4f * scale),
        screenRotation, position + screenFaceOffset);

    m_basicMeshes->DrawBoxMesh();

    // --- keyboard area --- dark outline on top of the base, sides only
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
    glm::vec3 keyboardOffset = ScaledOffset(rotation, scale, 0.0f, 0.06f, -0.3f);

    SetTransformations(glm::vec3(2.6f * scale, 0.01f * scale, 1.0f * scale),
        xRotation, yRotation, zRotation, position + keyboardOffset);

    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::left);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::right);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::front);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::back);

    // --- keyboard area --- dark outline on top of the base, sides only
    glm::vec3 mousePadOffset = ScaledOffset(rotation, scale, 0.0f, 0.06f, 0.65f);

    SetTransformations(glm::vec3(1.2f * scale, 0.01f * scale, 0.65f * scale),
        xRotation, yRotation, zRotation, position + mousePadOffset);

    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::left);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::right);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::front);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::back);
}