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
    Centerpiece(ShaderManager* shaderManager, ShapeMeshes* meshes, int branchTexture = 0, int cottonTexture = 0);

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

    void SetBranchTexture(int branchTexture) { m_branchTexture = branchTexture; }

    void SetCottonTexture(int cottonTexture) { m_cottonTexture = cottonTexture; }

private:
    int m_branchTexture = 0;
    int m_cottonTexture = 0;

    // Draws a single dark branch cylinder angled outward from the vase.
    // xRotation/yRotation control the tilt and direction of the branch.
    // xPosition/yPosition/zPosition offset the branch base from the vase top.
    // yScale controls the length of the branch.
    void RenderBranch(
        glm::vec3 position, float scale, glm::mat4 rotation,
        float xRotation, float yRotation,
        float xPosition, float yPosition, float zPosition,
        float yScale);

    // Draws a branch cylinder angled outward from the vase with a berry sphere
    // placed exactly at the tip. The tip position is derived from the branch
    // rotation matrix so the berry always lands correctly regardless of angle.
    // xRotation/yRotation control the tilt and direction of the branch.
    // xPosition/yPosition/zPosition offset the branch base from the vase top.
    // yScale controls the length of the branch.
    void RenderBranchWithBerry(glm::vec3 position, float scale, glm::mat4 rotation,
        float xRotation, float yRotation,
        float xPosition, float yPosition, float zPosition,
        float yScale);
};