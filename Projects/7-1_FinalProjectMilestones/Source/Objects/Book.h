#pragma once
#include "SceneObject.h"

/***********************************************************
 *  Book
 *
 *  Renders a 3D book object composed of a front cover,
 *  back cover, spine, and pages. A texture slot is passed
 *  in so multiple books can use different cover textures.
 ***********************************************************/
class Book : public SceneObject {
public:
    // constructor
    Book(ShaderManager* shaderManager, ShapeMeshes* meshes, int coverTextureSlot = 0, glm::vec2 uvScale = glm::vec2(1.0f, 1.0f));

    // Draws the book at the given world position.
    // All parts are positioned relative to the given position.
    //
    // position         - world space position of the book
    // scale            - uniform scale applied to all parts
    // coverTextureSlot - texture slot index for the front and back covers
    // xRotation        - rotation in degrees around the X axis
    // yRotation        - rotation in degrees around the Y axis
    // zRotation        - rotation in degrees around the Z axis
    void Render(
        glm::vec3 position = glm::vec3(0.0f),
        float scale = 1.0f,
        float xRotation = 0.0f,
        float yRotation = 0.0f,
        float zRotation = 0.0f) override;

    void SetCoverTexture(int coverTextureSlot) { m_coverTextureSlot = coverTextureSlot; };
    void SetUVScale(float u, float v) { m_uvScale = glm::vec2(u, v); };

private:
    int m_coverTextureSlot = 0;
    glm::vec2 m_uvScale = glm::vec2(1.0f, 1.0f);
};