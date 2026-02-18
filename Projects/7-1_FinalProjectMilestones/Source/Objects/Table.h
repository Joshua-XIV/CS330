#pragma once
#include "SceneObject.h"

class Table : public SceneObject {
public:
	Table(ShaderManager* shadeManager, ShapeMeshes* basicMeshes);

	void Render(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		float scale = 1.0f,
		float xRotation = 0.0f,
		float yRotation = 0.0f,
		float zRotation = 0.0f) override;
};