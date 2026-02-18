#pragma once
#include "SceneObject.h"

/***********************************************************
 *  Mug
 *
 *  This class handles the rendering of a 3D mug object
 *  composed of an outer body, inner wall, and handle.
 *  All parts are rendered relative to the given position
 *  so the entire mug moves and rotates as one unit.
 ***********************************************************/
class Mug : public SceneObject {
public:
    // constructor
    Mug(ShaderManager* shaderManager, ShapeMeshes* meshes);

    // Draws the mug at the given world position.
    // All parts (body, inner wall, handle) are positioned relative to the given position.
    //
    // position   - world space position of the mug base
    // scale      - scale applied to all parts
    // xRotation  - rotation in degrees around the X axis
    // yRotation  - rotation in degrees around the Y axis
    // zRotation  - rotation in degrees around the Z axis
    void Render(
        glm::vec3 position = glm::vec3(0.0f),
        float scale = 1.0f,
        float xRotation = 0.0f,
        float yRotation = 0.0f,
        float zRotation = 0.0f) override;
};