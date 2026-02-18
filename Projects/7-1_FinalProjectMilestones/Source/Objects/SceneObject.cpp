#include "SceneObject.h"
#include <glm/gtx/transform.hpp>

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