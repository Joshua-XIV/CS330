#include "SceneObject.h"
#include <glm/gtx/transform.hpp>

// shared materials reused across multiple objects.
// defined here so all derived classes have access without
// repeating the same magic values in each object's source file.
const SceneObject::ShaderMaterial SceneObject::MAT_SILVER = { glm::vec3(0.76f, 0.76f, 0.76f), glm::vec3(0.4f,  0.4f,  0.4f),   32.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_SCREEN = { glm::vec3(0.02f, 0.02f, 0.02f), glm::vec3(1.0f, 1.0f, 1.0f), 512.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_DECK_OUTLINE = { glm::vec3(0.15f, 0.15f, 0.15f), glm::vec3(0.05f, 0.05f, 0.05f),  4.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_DARK_KEY = { glm::vec3(0.2f,  0.2f,  0.2f),  glm::vec3(0.1f,  0.1f,  0.1f),   8.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_CENTERPIECE_BASE = { glm::vec3(0.15f, 0.15f, 0.18f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_CRYSTAL_BODY = { glm::vec3(1.0f, 1.0f, 1.0f),  glm::vec3(0.9f, 0.9f, 0.9f), 156.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_CRYSTAL_INNER = { glm::vec3(0.5f, 0.8f, 0.05f), glm::vec3(0.9f, 0.9f, 0.9f), 256.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_WOOD = { glm::vec3(0.35f, 0.22f, 0.1f),  glm::vec3(0.1f,  0.08f, 0.05f),  16.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_TEAL = { glm::vec3(0.4f,  0.55f, 0.5f),  glm::vec3(0.05f, 0.05f, 0.05f),  4.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_BROWN = { glm::vec3(0.545f,0.271f,0.075f),glm::vec3(0.545f,0.271f,0.075f), 4.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_COASTER = { glm::vec3(0.7f, 0.65f, 0.6f), glm::vec3(0.02f, 0.02f, 0.02f), 2.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_BOOK_COVER = { glm::vec3(0.3f, 0.25f, 0.2f),  glm::vec3(0.05f, 0.05f, 0.05f), 4.0f };
const SceneObject::ShaderMaterial SceneObject::MAT_BOOK_PAGES = { glm::vec3(0.95f, 0.92f, 0.85f), glm::vec3(0.05f, 0.05f, 0.05f), 4.0f };

/***********************************************************
 *  SetTransformations()
 *
 *  Builds and uploads the model matrix from X, Y, Z
 *  rotation values. Rotation order is Z * Y * X.
 ***********************************************************/
void SceneObject::SetTransformations(glm::vec3 scaleXYZ, float rotX, float rotY, float rotZ, glm::vec3 positionXYZ)
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
 *  Used when an mesh is rotated by default, so it can match rotaions passed in
 ***********************************************************/
void SceneObject::SetTransformations(glm::vec3 scaleXYZ, glm::mat4 rotation, glm::vec3 positionXYZ)
{
    glm::mat4 scale = glm::scale(scaleXYZ);
    glm::mat4 translation = glm::translate(positionXYZ);
    glm::mat4 modelView = translation * rotation * scale;
    m_pShaderManager->setMat4Value("model", modelView);
}

/***********************************************************
 *  BuildRotationMatrix()
 *
 *  Builds and returns a rotation matrix from X, Y, Z
 *  rotation values. Rotation order is Z * Y * X.
 ***********************************************************/
glm::mat4 SceneObject::BuildRotationMatrix(float xRotation, float yRotation, float zRotation)
{
    glm::mat4 rotX = glm::rotate(glm::radians(xRotation), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotY = glm::rotate(glm::radians(yRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotZ = glm::rotate(glm::radians(zRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    return rotZ * rotY * rotX;
}