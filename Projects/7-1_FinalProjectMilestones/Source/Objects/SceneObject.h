#pragma once
#include <glm/glm.hpp>
#include "ShaderManager.h"
#include "ShapeMeshes.h"

/***********************************************************
 *  SceneObject
 *
 *  Base class for all renderable objects in the 3D scene.
 *  Provides shared shader and mesh references, as well as
 *  shared transformation utilities so derived classes only
 *  need to implement their own Render() method.
 * 
 *  I decided to make this class while building my first object
 *  because I didn't want to go and change the positions of so many
 *  different meshes along with rotations for a single object,
 *  so all I have to do is build it once and everything 
 *  transforms from the Render function all at once together.
 ***********************************************************/
class SceneObject {
public:
    // constructor - stores shader manager and mesh references
    SceneObject(ShaderManager* shaderManager, ShapeMeshes* meshes)
        : m_pShaderManager(shaderManager), m_basicMeshes(meshes) {
    }

    // pure virtual - all derived classes must implement Render()
    //
    // position   - world space position of the object
    // scale      - uniform scale applied to all parts
    // xRotation  - rotation in degrees around the X axis
    // yRotation  - rotation in degrees around the Y axis
    // zRotation  - rotation in degrees around the Z axis
    virtual void Render(
        glm::vec3 position = glm::vec3(0.0f),
        float scale = 1.0f,
        float xRotation = 0.0f,
        float yRotation = 0.0f,
        float zRotation = 0.0f) = 0;

protected:
    ShaderManager* m_pShaderManager;
    ShapeMeshes* m_basicMeshes;


    /***********************************************************
     *  ShaderMaterial
     *
     *  Holds the three material properties used by the
     *  lighting shader. Used with SetShaderMaterial() to apply
     *  a material to the next draw call in one step rather than
     *  setting each shader individually. 
     *  (similiar to the OpenGLSample project)
     ***********************************************************/
    struct ShaderMaterial {
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
    };

    // shared materials reused across multiple objects.
    // defined here so all derived classes have access without
    // repeating the same magic values in each object's source file.
    // static since we only need one instance of each material to apply
    // which helps reduce memory
    static const ShaderMaterial MAT_SILVER;
    static const ShaderMaterial MAT_SCREEN;
    static const ShaderMaterial MAT_DECK_OUTLINE;
    static const ShaderMaterial MAT_DARK_KEY;
    static const ShaderMaterial MAT_CENTERPIECE_BASE;
    static const ShaderMaterial MAT_CRYSTAL_BODY;
    static const ShaderMaterial MAT_CRYSTAL_INNER;
    static const ShaderMaterial MAT_WOOD;
    static const ShaderMaterial MAT_TEAL;
    static const ShaderMaterial MAT_BROWN;
    static const ShaderMaterial MAT_COASTER;
    static const ShaderMaterial MAT_BOOK_COVER;
    static const ShaderMaterial MAT_BOOK_PAGES;

    /***********************************************************
     *  SetShaderMaterial()
     *
     *  wrapper to upload diffuse color, specular color,
     *  and shininess into the shader in one call.
     *
     *  mat - the ShaderMaterial to apply to the next draw call
     ***********************************************************/
    void SetShaderMaterial(const ShaderMaterial& mat) {
        m_pShaderManager->setVec3Value("material.diffuseColor", mat.diffuse);
        m_pShaderManager->setVec3Value("material.specularColor", mat.specular);
        m_pShaderManager->setFloatValue("material.shininess", mat.shininess);
    }

    // builds and uploads the model matrix from X, Y, Z
    // rotation values. Rotation order is Z * Y * X.
    void SetTransformations(glm::vec3 scaleXYZ, float rotX, float rotY, float rotZ, glm::vec3 positionXYZ);

    // overload that accepts a precomputed rotation matrix.
    // used when euler angle conversion would cause issues.
    void SetTransformations(glm::vec3 scaleXYZ, glm::mat4 rotation, glm::vec3 positionXYZ);

    // builds a rotation matrix from X, Y, Z rotation values.
    // rotation order is Z * Y * X.
    glm::mat4 BuildRotationMatrix(float xRotation, float yRotation, float zRotation);

    // returns a rotation-transformed and scaled offset vector.
    // only rotation is applied, not translation.
    glm::vec3 ScaledOffset(glm::mat4 rotation, float scale, float x, float y, float z) {
        return glm::vec3(rotation * glm::vec4(x * scale, y * scale, z * scale, 0.0f));
    }
};