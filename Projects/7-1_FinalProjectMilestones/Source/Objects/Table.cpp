#include "Table.h"
#include <glm/gtx/transform.hpp>

/***********************************************************
 *  Table()
 *
 *  Constructor - passes shader manager and meshes up to
 *  the SceneObject base class.
 ***********************************************************/
Table::Table(ShaderManager* shaderManager, ShapeMeshes* basicMeshes) : SceneObject(shaderManager, basicMeshes) {}

/***********************************************************
 *  Render()
 *
 *  Draws all parts of the table all relative to the given position.
 *
 *  Multiple parts are offset from a given position to create the
 *  entire object as one unit. This also means rotations were applied
 *  and rotated from that relative position.
 ***********************************************************/
void Table::Render(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation) {
    // build rotation matrix for transforming offsets and the handle
    glm::mat4 rotation = BuildRotationMatrix(xRotation, yRotation, zRotation);

    // table_leg texture at slot 2, shared across all legs
    m_pShaderManager->setIntValue("bUseTexture", true);
    m_pShaderManager->setSampler2DValue("objectTexture", 2);
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(1.0f, 1.0f));

    float SLANT_LEG_Y_SCALE = 2.5f;
    float SLANT_LEG_X_OFFSET = 0.7f;        // horizontal distance from center
    float BOTTOM_SLANT_LEG_Y_OFFSET = 1.0f; // vertical start of bottom legs
    float UPPER_SLANT_LEG_Y_OFFSET = 4.0f;  // vertical start of upper legs
    float SLANT_LEG_Z_OFFSET = 0.7f;        // depth distance from center
    float SLANT_LEG_ANGLE = 60.0f;          // angle of leg slant in degrees

    // --- bottom front left leg --- slants outward toward front left
    glm::mat4 frontLeftLegBaseRot = glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(-1.0f, 0.0f, -1.0f));
    glm::mat4 frontLeftLegRot = rotation * frontLeftLegBaseRot;

    // offset -X and +Z to front left corner
    glm::vec3 frontLeftLegOffset = ScaledOffset(rotation, scale, -SLANT_LEG_X_OFFSET, BOTTOM_SLANT_LEG_Y_OFFSET, SLANT_LEG_X_OFFSET);
    SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
        frontLeftLegRot, position + frontLeftLegOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- bottom front right leg --- slants outward toward front right
    glm::mat4 frontRightLegBaseRot = glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(-1.0f, 0.0f, 1.0f));
    glm::mat4 frontRightLegRot = rotation * frontRightLegBaseRot;

    // offset +X and +Z to front right corner
    glm::vec3 frontRightLegOffset = ScaledOffset(rotation, scale, SLANT_LEG_X_OFFSET, BOTTOM_SLANT_LEG_Y_OFFSET, SLANT_LEG_X_OFFSET);
    SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
        frontRightLegRot, position + frontRightLegOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- bottom back left leg --- slants outward toward back left
    glm::mat4 backLeftLegBaseRot = glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(1.0f, 0.0f, -1.0f));
    glm::mat4 backLeftLegRot = rotation * backLeftLegBaseRot;

    // offset -X and -Z to back left corner
    glm::vec3 backLeftLegOffset = ScaledOffset(rotation, scale, -SLANT_LEG_X_OFFSET, BOTTOM_SLANT_LEG_Y_OFFSET, -SLANT_LEG_X_OFFSET);
    SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
        backLeftLegRot, position + backLeftLegOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- bottom back right leg --- slants outward toward back right
    glm::mat4 backRightLegBaseRot = glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(1.0f, 0.0f, 1.0f));
    glm::mat4 backRightLegRot = rotation * backRightLegBaseRot;

    // offset +X and -Z to back right corner
    glm::vec3 backRightLegOffset = ScaledOffset(rotation, scale, SLANT_LEG_X_OFFSET, BOTTOM_SLANT_LEG_Y_OFFSET, -SLANT_LEG_X_OFFSET);
    SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
        backRightLegRot, position + backRightLegOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- middle leg --- vertical center post, offset 2.5 up
    glm::vec3 middleLegOffset = ScaledOffset(rotation, scale, 0.0f, 2.5f, 0.0f);
    SetTransformations(glm::vec3(1.0f * scale, 3.0 * scale, 1.0f * scale),
        xRotation, yRotation, zRotation, position + middleLegOffset);
    m_basicMeshes->DrawBoxMesh();

    // 180 Y flip so upper legs mirror the bottom legs outward from center
    glm::mat4 yAxisFlip = glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // --- upper front left leg --- splays outward from middle leg
    glm::mat4 upperFrontLeftLegBaseRot = yAxisFlip * glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(1.0f, 0.0f, 1.0f));
    glm::mat4 upperFrontLeftLegRot = rotation * upperFrontLeftLegBaseRot;

    // offset +X and -Z to front left, 4.0 up
    glm::vec3 upperFrontLeftLegOffset = ScaledOffset(rotation, scale, SLANT_LEG_X_OFFSET, UPPER_SLANT_LEG_Y_OFFSET, -SLANT_LEG_Z_OFFSET);
    SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
        upperFrontLeftLegRot, position + upperFrontLeftLegOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- upper front right leg --- splays outward from middle leg
    glm::mat4 upperFrontRightLegBaseRot = yAxisFlip * glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(1.0f, 0.0f, -1.0f));
    glm::mat4 upperFrontRightLegRot = rotation * upperFrontRightLegBaseRot;

    // offset -X and -Z to front right, 4.0 up
    glm::vec3 upperFrontRightLegOffset = ScaledOffset(rotation, scale, -SLANT_LEG_X_OFFSET, UPPER_SLANT_LEG_Y_OFFSET, -SLANT_LEG_Z_OFFSET);
    SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
        upperFrontRightLegRot, position + upperFrontRightLegOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- upper back left leg --- splays outward from middle leg
    glm::mat4 upperBackLeftLegBaseRot = yAxisFlip * glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(-1.0f, 0.0f, 1.0f));
    glm::mat4 upperBackLeftLegRot = rotation * upperBackLeftLegBaseRot;

    // offset +X and +Z to back left, 4.0 up
    glm::vec3 upperBackLeftLegOffset = ScaledOffset(rotation, scale, SLANT_LEG_X_OFFSET, UPPER_SLANT_LEG_Y_OFFSET, SLANT_LEG_Z_OFFSET);
    SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
        upperBackLeftLegRot, position + upperBackLeftLegOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- upper back right leg --- splays outward from middle leg
    glm::mat4 upperBackRightLegBaseRot = yAxisFlip * glm::rotate(glm::radians(SLANT_LEG_ANGLE), glm::vec3(-1.0f, 0.0f, -1.0f));
    glm::mat4 upperBackRightLegRot = rotation * upperBackRightLegBaseRot;

    // offset -X and +Z to back right, 4.0 up
    glm::vec3 upperBackRightLegOffset = ScaledOffset(rotation, scale, -SLANT_LEG_X_OFFSET, UPPER_SLANT_LEG_Y_OFFSET, SLANT_LEG_Z_OFFSET);
    SetTransformations(glm::vec3(1.0f * scale, SLANT_LEG_Y_SCALE * scale, 1.0f * scale),
        upperBackRightLegRot, position + upperBackRightLegOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- table top --- flat cylinder, table_wood texture at slot 1
    m_pShaderManager->setSampler2DValue("objectTexture", 1);
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(1.0f, 1.0f));

    // offset 5.03 up to sit above all legs
    glm::vec3 tableTopOffset = ScaledOffset(rotation, scale, 0.0f, 5.03f, 0.0f);
    SetTransformations(glm::vec3(6.0f * scale, 0.2f * scale, 6.0f * scale),
        xRotation, yRotation, zRotation, position + tableTopOffset);
    m_basicMeshes->DrawCylinderMesh();
}