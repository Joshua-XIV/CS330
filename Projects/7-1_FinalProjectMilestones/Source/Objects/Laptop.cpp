#include "Laptop.h"
#include <glm/gtx/transform.hpp>

/***********************************************************
 *  Laptop()
 *
 *  Constructor - passes shader manager and meshes up to
 *  the SceneObject base class.
 ***********************************************************/
Laptop::Laptop(ShaderManager* shaderManager, ShapeMeshes* meshes, int laptopFrameTexture, int keyTexture)
    : SceneObject(shaderManager, meshes), m_laptopFrameTexture(laptopFrameTexture), m_keyTexture(keyTexture) {}

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

    // enable textures for the aluminum body panels
    m_pShaderManager->setIntValue("bUseTexture", true);
    m_pShaderManager->setSampler2DValue("objectTexture", m_laptopFrameTexture);

    // --- base / keyboard deck --- flat silver box, no offset
    SetShaderMaterial(MAT_SILVER);
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

    // disable textures for remaining parts
    m_pShaderManager->setIntValue("bUseTexture", false);

    // --- screen outline --- near-black border around the screen panel, sides only
    SetShaderMaterial(MAT_SCREEN);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));

    glm::vec3 screenOutlineOffset = ScaledOffset(rotation, scale, 0.0f, 0.8f, -1.05f);
    SetTransformations(glm::vec3(2.9f * scale, 0.03f * scale, 1.44f * scale),
        screenRotation, position + screenOutlineOffset);

    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::left);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::right);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::front);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::back);

    // --- screen face --- near-black box on the inner face of the screen panel
    glm::vec3 screenFaceOffset = ScaledOffset(rotation, scale, 0.0f, 0.8f, -1.04f);
    SetTransformations(glm::vec3(2.8f * scale, 0.01f * scale, 1.4f * scale),
        screenRotation, position + screenFaceOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- keyboard area --- dark outline on top of the base, sides only
    SetShaderMaterial(MAT_DECK_OUTLINE);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));

    glm::vec3 keyboardOffset = ScaledOffset(rotation, scale, 0.0f, 0.06f, -0.275f);
    SetTransformations(glm::vec3(2.6f * scale, 0.01f * scale, 1.05f * scale),
        xRotation, yRotation, zRotation, position + keyboardOffset);

    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::left);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::right);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::front);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::back);

    // --- mousepad outline --- dark outline on top of the base, sides only
    glm::vec3 mousePadOffset = ScaledOffset(rotation, scale, 0.0f, 0.06f, 0.65f);
    SetTransformations(glm::vec3(1.2f * scale, 0.01f * scale, 0.65f * scale),
        xRotation, yRotation, zRotation, position + mousePadOffset);

    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::left);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::right);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::front);
    m_basicMeshes->DrawBoxMeshSide(ShapeMeshes::back);

    RenderKeyboard(position, scale, xRotation, yRotation, zRotation);
}

/***********************************************************
 *  RenderKeyboard()
 *
 *  Draws all keys of the laptop keyboard relative to the
 *  given position. This will be called in Render.
 *
 *  Six rows of keys are drawn using box meshes offset from
 *  the base deck position. Rows increase in Z from back to
 *  front. Wide modifier keys anchor each row on the left or
 *  right. The bottom row includes a spacebar and stacked
 *  arrow keys.
 ***********************************************************/
void Laptop::RenderKeyboard(glm::vec3 position, float scale, float xRotation, float yRotation, float zRotation) {
    glm::mat4 rotation = BuildRotationMatrix(xRotation, yRotation, zRotation);

    // enable textures and apply key material for all keys
    m_pShaderManager->setIntValue("bUseTexture", true);
    m_pShaderManager->setSampler2DValue("objectTexture", m_keyTexture);
    SetShaderMaterial(MAT_DARK_KEY);
    m_pShaderManager->setVec4Value("objectColor", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

    float keyH = 0.062f;        // Y offset above deck
    float sqW = 0.155f;         // square key X
    float sqD = 0.155f;         // square key Z
    float topD = 0.09f;         // top row key Z - skinny
    float arrD = 0.07f;         // arrow key Z - skinny
    float gap = 0.018f;         // gap between keys
    float step = sqW + gap;
    float wideW = sqW * 1.8f;
    float widerW = sqW * 1.95f;
    float widestW = sqW * 2.50f;

    // total keyboard width anchors to keyboard outline (2.6 * scale wide, centered)
    float totalW = 13 * step + wideW;
    float startX = -totalW / 2.0f;

    // --- row 1 --- 13 skinny keys + 1 backspace (same Z, wider X)
    float row1Z = -0.72f;
    for (int i = 0; i < 13; i++) {
        float xPos = startX + i * step + sqW / 2.0f;
        glm::vec3 offset = ScaledOffset(rotation, scale, xPos, keyH, row1Z);
        SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, topD * scale),
            xRotation, yRotation, zRotation, position + offset);
        m_basicMeshes->DrawBoxMesh();
    }
    glm::vec3 bsOffset = ScaledOffset(rotation, scale, startX + 13 * step + wideW / 2.0f, keyH, row1Z);
    SetTransformations(glm::vec3(wideW * scale, 0.02f * scale, topD * scale),
        xRotation, yRotation, zRotation, position + bsOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- row 2 --- 13 square keys + 1 wide last key
    float row2Z = row1Z + (sqD + gap);
    for (int i = 0; i < 13; i++) {
        float xPos = startX + i * step + sqW / 2.0f;
        glm::vec3 offset = ScaledOffset(rotation, scale, xPos, keyH, row2Z);
        SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, sqD * scale),
            xRotation, yRotation, zRotation, position + offset);
        m_basicMeshes->DrawBoxMesh();
    }
    glm::vec3 row2EndOffset = ScaledOffset(rotation, scale, startX + 13 * step + wideW / 2.0f, keyH, row2Z);
    SetTransformations(glm::vec3(wideW * scale, 0.02f * scale, sqD * scale),
        xRotation, yRotation, zRotation, position + row2EndOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- row 3 --- 1 wide first key + 13 square keys
    float row3Z = row2Z + (sqD + gap);
    glm::vec3 row3StartOffset = ScaledOffset(rotation, scale, startX + wideW / 2.0f, keyH, row3Z);
    SetTransformations(glm::vec3(wideW * scale, 0.02f * scale, sqD * scale),
        xRotation, yRotation, zRotation, position + row3StartOffset);
    m_basicMeshes->DrawBoxMesh();
    for (int i = 0; i < 13; i++) {
        float xPos = startX + wideW + gap + i * step + sqW / 2.0f;
        glm::vec3 offset = ScaledOffset(rotation, scale, xPos, keyH, row3Z);
        SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, sqD * scale),
            xRotation, yRotation, zRotation, position + offset);
        m_basicMeshes->DrawBoxMesh();
    }

    // --- row 4 --- wide first, 11 square, wide last
    float row4Z = row3Z + (sqD + gap);
    glm::vec3 row4StartOffset = ScaledOffset(rotation, scale, startX + widerW / 2.0f, keyH, row4Z);
    SetTransformations(glm::vec3(widerW * scale, 0.02f * scale, sqD * scale),
        xRotation, yRotation, zRotation, position + row4StartOffset);
    m_basicMeshes->DrawBoxMesh();
    float row4KeyStart = startX + widerW + gap;
    for (int i = 0; i < 11; i++) {
        float xPos = row4KeyStart + i * step + sqW / 2.0f;
        glm::vec3 offset = ScaledOffset(rotation, scale, xPos, keyH, row4Z);
        SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, sqD * scale),
            xRotation, yRotation, zRotation, position + offset);
        m_basicMeshes->DrawBoxMesh();
    }
    float row4EndX = row4KeyStart + 11 * step + widerW / 2.0f;
    glm::vec3 row4EndOffset = ScaledOffset(rotation, scale, row4EndX, keyH, row4Z);
    SetTransformations(glm::vec3(widerW * scale, 0.02f * scale, sqD * scale),
        xRotation, yRotation, zRotation, position + row4EndOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- row 5 --- widest first, 10 square, widest last
    float row5Z = row4Z + (sqD + gap);
    glm::vec3 row5StartOffset = ScaledOffset(rotation, scale, startX + widestW / 2.0f, keyH, row5Z);
    SetTransformations(glm::vec3(widestW * scale, 0.02f * scale, sqD * scale),
        xRotation, yRotation, zRotation, position + row5StartOffset);
    m_basicMeshes->DrawBoxMesh();
    float row5KeyStart = startX + widestW + gap;
    for (int i = 0; i < 10; i++) {
        float xPos = row5KeyStart + i * step + sqW / 2.0f;
        glm::vec3 offset = ScaledOffset(rotation, scale, xPos, keyH, row5Z);
        SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, sqD * scale),
            xRotation, yRotation, zRotation, position + offset);
        m_basicMeshes->DrawBoxMesh();
    }
    float row5EndX = row5KeyStart + 10 * step + widestW / 2.0f;
    glm::vec3 row5EndOffset = ScaledOffset(rotation, scale, row5EndX, keyH, row5Z);
    SetTransformations(glm::vec3(widestW * scale, 0.02f * scale, sqD * scale),
        xRotation, yRotation, zRotation, position + row5EndOffset);
    m_basicMeshes->DrawBoxMesh();

    // --- row 6 --- 4 square, spacebar, 2 square, arrow keys
    float row6Z = row5Z + (sqD + gap);

    // 4 square keys on the left
    for (int i = 0; i < 4; i++) {
        float xPos = startX + i * step + sqW / 2.0f;
        glm::vec3 offset = ScaledOffset(rotation, scale, xPos, keyH, row6Z);
        SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, sqD * scale),
            xRotation, yRotation, zRotation, position + offset);
        m_basicMeshes->DrawBoxMesh();
    }

    // spacebar - long X
    // adjusted long enough to have arrow keys line up with the edge of other keys
    float spaceW = 5.62f * step - gap;
    float spaceX = startX + 4 * step + spaceW / 2.0f;
    glm::vec3 spaceOffset = ScaledOffset(rotation, scale, spaceX, keyH, row6Z);
    SetTransformations(glm::vec3(spaceW * scale, 0.02f * scale, sqD * scale),
        xRotation, yRotation, zRotation, position + spaceOffset);
    m_basicMeshes->DrawBoxMesh();

    // 2 square keys after spacebar
    float afterSpaceX = startX + 4 * step + spaceW + gap;
    for (int i = 0; i < 2; i++) {
        float xPos = afterSpaceX + i * step + sqW / 2.0f;
        glm::vec3 offset = ScaledOffset(rotation, scale, xPos, keyH, row6Z);
        SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, sqD * scale),
            xRotation, yRotation, zRotation, position + offset);
        m_basicMeshes->DrawBoxMesh();
    }

    // arrow keys - skinny Z, clustered at the right end of row 6
    // arrShift pushes left/down/right forward, upShift pulls up arrow back
    float arrStartX = afterSpaceX + 2 * step;
    float arrShift = (arrD + gap) * 0.5f;
    float upShift = (arrD + gap) * -0.4f;

    // left arrow
    glm::vec3 leftArrOffset = ScaledOffset(rotation, scale, arrStartX + sqW / 2.0f, keyH, row6Z + arrShift);
    SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, arrD * scale),
        xRotation, yRotation, zRotation, position + leftArrOffset);
    m_basicMeshes->DrawBoxMesh();

    // up arrow - sits on top half of the left/right arrow row
    float upDownX = arrStartX + step + sqW / 2.0f;
    glm::vec3 upArrOffset = ScaledOffset(rotation, scale, upDownX, keyH, row6Z + upShift);
    SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, arrD * scale),
        xRotation, yRotation, zRotation, position + upArrOffset);
    m_basicMeshes->DrawBoxMesh();

    // down arrow - sits on bottom half
    glm::vec3 downArrOffset = ScaledOffset(rotation, scale, upDownX, keyH, row6Z + arrShift);
    SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, arrD * scale),
        xRotation, yRotation, zRotation, position + downArrOffset);
    m_basicMeshes->DrawBoxMesh();

    // right arrow
    glm::vec3 rightArrOffset = ScaledOffset(rotation, scale, arrStartX + 2 * step + sqW / 2.0f, keyH, row6Z + arrShift);
    SetTransformations(glm::vec3(sqW * scale, 0.02f * scale, arrD * scale),
        xRotation, yRotation, zRotation, position + rightArrOffset);
    m_basicMeshes->DrawBoxMesh();

    // disable textures for next parts/items
    m_pShaderManager->setIntValue("bUseTexture", false);
}