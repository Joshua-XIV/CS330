#pragma once
#include <glm/glm.hpp>
#include "ShaderManager.h"
#include "ShapeMeshes.h"

/***********************************************************
 *  Mug
 *
 *  This class handles the rendering of a 3D mug object
 *  composed of an outer body, inner wall, and handle.
 *  All parts are rendered relative to the given position
 *  so the entire mug moves and rotates as one unit.
 ***********************************************************/
class Mug {
public:
    // constructor
    Mug(ShaderManager* shaderManager, ShapeMeshes* meshes);

    /***********************************************************
     *  Render()
     *
     *  Draws the mug at the given world position with the
     *  given scale and rotation. All parts (body, inner wall,
     *  handle) are positioned relative to the given position.
     *
     *  position    - world space position of the mug base
     *  scale       - scale applied to all parts
     *  xRotation   - rotation in degrees around the X axis
     *  yRotation   - rotation in degrees around the Y axis
     *  zRotation   - rotation in degrees around the Z axis
     ***********************************************************/
    void Render(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0), float scale = 1, float xRotation = 0, float yRotation = 0, float zRotation = 0);

private:
    ShaderManager* m_pShaderManager;
    ShapeMeshes* m_basicMeshes;

    /***********************************************************
     *  SetTransformations()
     *
     *  Builds and uploads the model matrix from X, Y, Z
     *  rotation values. Rotation order is Z * Y * X.
     *  Similar to SceneManager's Set Transformations
     ***********************************************************/
    void SetTransformations(glm::vec3 scale, float rotX, float rotY, float rotZ, glm::vec3 pos);

    /***********************************************************
     *  SetTransformations()
     *
     *  Overload that accepts a precomputed rotation matrix.
     *  Used for the handle to avoid rotation conversion issues.
     ***********************************************************/
    void SetTransformations(glm::vec3 scaleXYZ, glm::mat4 rotation, glm::vec3 positionXYZ);
};