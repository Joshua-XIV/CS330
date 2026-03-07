#include "Centerpiece.h"
#include <glm/gtx/transform.hpp>

/***********************************************************
 *  Centerpiece()
 *
 *  Constructor - passes shader manager and meshes up to
 *  the SceneObject base class.
 ***********************************************************/
Centerpiece::Centerpiece(ShaderManager* shaderManager, ShapeMeshes* meshes, int branchTexture, int cottonTexture) 
    : SceneObject(shaderManager, meshes), m_branchTexture(branchTexture), m_cottonTexture(cottonTexture){}

/***********************************************************
 *  Render()
 *
 *  Draws all parts of the crystal prism centerpiece relative
 *  to the given position.
 *
 *  Multiple parts are offset from a given position to create
 *  the entire object as one unit. Rotations are applied and
 *  rotated from that relative position.
 ***********************************************************/
void Centerpiece::Render(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation) {
    glm::mat4 rotation = BuildRotationMatrix(xRotation, yRotation, zRotation);

    m_pShaderManager->setIntValue("bUseTexture", false);

    // --- flat base --- thin dark cylinder the prism rests on
    SetShaderMaterial(MAT_CENTERPIECE_BASE);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.15f, 0.15f, 0.18f, 1.0f));

    // no offset, base sits at position
    SetTransformations(glm::vec3(0.35f * scale, 0.04f * scale, 0.35f * scale),
        xRotation, yRotation, zRotation, position);
    m_basicMeshes->DrawCylinderMesh(true, true, true);

    // --- prism body --- crystal glass triangular prism standing upright
    SetShaderMaterial(MAT_CRYSTAL_BODY);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.4f, 0.55f, 0.6f, 0.3f));

    // offset up so it sits on top of the base
    glm::vec3 prismOffset = ScaledOffset(rotation, scale, 0.0f, 0.75f, 0.08f);
    SetTransformations(glm::vec3(0.56f * scale, 1.4f * scale, 0.56f * scale),
        xRotation, yRotation, zRotation, position + prismOffset);
    m_basicMeshes->DrawPrismMesh();

    // --- inner highlight --- slightly smaller prism inside main body
    SetShaderMaterial(MAT_CRYSTAL_INNER);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.5f, 0.7f, 0.75f, 0.5f));

    // same offset as prism body, slightly smaller scale
    SetTransformations(glm::vec3(0.36f * scale, 1.36f * scale, 0.36f * scale),
        xRotation, yRotation, zRotation, position + prismOffset);
    m_basicMeshes->DrawPrismMesh();

    // ---------------------------------------------------------------
    // plant - dark branches angling outward with white berry spheres
    // branches all start near the top of the prism
    // ---------------------------------------------------------------

    float BRANCH_Y_HEIGHT = 1.40f;
    float STEM_LENGTH = 0.6f;
    float STEM_TIP_Y = BRANCH_Y_HEIGHT + STEM_LENGTH;
    m_pShaderManager->setIntValue("bUseTexture", true);

    // --- center main stem --- straight up
    RenderBranch(position, scale, rotation, 0.0f, 0.0f, 0.0f, BRANCH_Y_HEIGHT, 0.0f, STEM_LENGTH);

    // --- branch 1 --- angles forward-left from stem tip
    RenderBranchWithBerry(position, scale, rotation, -30.0f, 40.0f, 0.0f, STEM_TIP_Y, 0.0f, 0.7f);
    // --- branch 2 --- angles forward-right from stem tip
    RenderBranchWithBerry(position, scale, rotation, -30.0f, -40.0f, 0.0f, STEM_TIP_Y, 0.0f, 1.1f);
    // --- branch 3 --- angles back-left from stem tip
    RenderBranchWithBerry(position, scale, rotation, -25.0f, 130.0f, 0.0f, STEM_TIP_Y, 0.0f, 0.6f);
    // --- branch 4 --- angles back-right from stem tip
    RenderBranchWithBerry(position, scale, rotation, -25.0f, 220.0f, 0.0f, STEM_TIP_Y, 0.0f, 1.3f);
    // --- branch 5 --- tall lean to the left from stem tip
    RenderBranchWithBerry(position, scale, rotation, -20.0f, 90.0f, 0.0f, STEM_TIP_Y, 0.0f, 0.9f);

    // --- lower tier --- shallower angles so they spread outward and avoid the upper tier
    // --- lower branch 1 --- angles forward-left
    RenderBranchWithBerry(position, scale, rotation, -55.0f, 40.0f, 0.0f, BRANCH_Y_HEIGHT + 0.1f, 0.0f, 0.5f);
    // --- lower branch 2 --- angles forward-right
    RenderBranchWithBerry(position, scale, rotation, -60.0f, -40.0f, 0.0f, BRANCH_Y_HEIGHT + 0.3f, 0.0f, 0.8f);
    // --- lower branch 3 --- angles back-left
    RenderBranchWithBerry(position, scale, rotation, -50.0f, 130.0f, 0.0f, BRANCH_Y_HEIGHT + 0.5f, 0.0f, 0.45f);
    // --- lower branch 4 --- angles back-right
    RenderBranchWithBerry(position, scale, rotation, -65.0f, 220.0f, 0.0f, BRANCH_Y_HEIGHT + 0.2f, 0.0f, 0.95f);
    // --- lower branch 5 --- lean to the left
    RenderBranchWithBerry(position, scale, rotation, -55.0f, 90.0f, 0.0f, BRANCH_Y_HEIGHT + 0.4f, 0.0f, 0.65f);

}

/***********************************************************
 *  RenderBranch()
 *
 *  Draws a single dark branch cylinder angled outward from
 *  the vase. xRotation and yRotation control the tilt and
 *  direction of the branch. xPosition/yPosition/zPosition
 *  offset the branch base from the vase top. yScale controls
 *  the length of the branch.
 ***********************************************************/
void Centerpiece::RenderBranch(glm::vec3 position, float scale, glm::mat4 rotation, float xRotation, float yRotation,
    float xPosition, float yPosition, float zPosition,
    float yScale)
{

    SetShaderMaterial(MAT_WOOD);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.12f, 0.08f, 0.05f, 1.0f));
    m_pShaderManager->setSampler2DValue("objectTexture", m_branchTexture);
    glm::mat4 yaw = glm::rotate(glm::radians(yRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 tilt = glm::rotate(glm::radians(xRotation), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 branchRotation = rotation * yaw * tilt;

    glm::vec3 branchOffset = ScaledOffset(rotation, scale, xPosition, yPosition, zPosition);
    SetTransformations(glm::vec3(0.03f * scale, yScale * scale, 0.03f * scale),
        branchRotation, position + branchOffset);
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(3.0f, 1.0f));
    m_basicMeshes->DrawCylinderMesh(true, true, true);
}

/***********************************************************
 *  RenderBerry()
 *
 *  Draws a single white berry sphere at the tip of a branch.
 *  xPosition/yPosition/zPosition offset the berry from the
 *  vase top.
 ***********************************************************/
void Centerpiece::RenderBranchWithBerry(glm::vec3 position, float scale, glm::mat4 rotation,
    float xRotation, float yRotation,
    float xPosition, float yPosition, float zPosition,
    float yScale)
{
    glm::mat4 yaw = glm::rotate(glm::radians(yRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 tilt = glm::rotate(glm::radians(xRotation), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 branchRotation = rotation * yaw * tilt;

    glm::vec3 branchOffset = ScaledOffset(rotation, scale, xPosition, yPosition, zPosition);

    // --- branch cylinder ---
    SetShaderMaterial(MAT_WOOD);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.12f, 0.08f, 0.05f, 1.0f));
    m_pShaderManager->setSampler2DValue("objectTexture", m_branchTexture);
    SetTransformations(glm::vec3(0.03f * scale, yScale * scale, 0.03f * scale),
        branchRotation, position + branchOffset);
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(3.0f, 1.0f));
    m_basicMeshes->DrawCylinderMesh(true, true, true);

    // --- berry at tip --- offset along the branch direction by yScale
    glm::vec3 tipOffset = glm::vec3(branchRotation * glm::vec4(0.0f, yScale * scale, 0.0f, 0.0f));
    SetShaderMaterial(MAT_CRYSTAL_BODY);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.92f, 0.90f, 0.88f, 1.0f));
    m_pShaderManager->setSampler2DValue("objectTexture", m_cottonTexture);
    SetTransformations(glm::vec3(0.07f * scale, 0.07f * scale, 0.07f * scale),
        rotation, position + branchOffset + tipOffset);
    m_pShaderManager->setVec2Value("UVscale", glm::vec2(1.0f, 1.0f));
    m_basicMeshes->DrawSphereMesh();
}
