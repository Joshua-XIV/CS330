#pragma once
#include "SceneObject.h"

/***********************************************************
 *  Centerpiece
 *
 *  A decorative glass vase centerpiece composed of a narrow
 *  cylinder neck, flared cone body, and flat cylinder base.
 *  Rendered with high specular and low diffuse to mimic glass.
 ***********************************************************/
class Centerpiece : public SceneObject {
public:
    Centerpiece(ShaderManager* shaderManager, ShapeMeshes* meshes);

    // Draws the center piece at the given world position.
    // All parts are positioned relative to the given position.
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