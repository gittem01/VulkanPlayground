#pragma once

#include "btBulletDynamicsCommon.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include <vector>

struct RenderObject {
	std::string meshName;
	std::string materialName;
	std::string textureName;

	glm::vec4 color;
};

class GameObject {

public:

	void* physicsWorld;

	RenderObject* renderObject;
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scl;

	glm::mat4 objectMatrix; // transform * rotation * scale
	glm::mat4 rotationMatrix;

	btRigidBody* rigidBody;

	GameObject(void* physicsWorld,
		glm::vec3 position = glm::vec3(),
		glm::vec3 rotation = glm::vec3(),
		glm::vec3 scale = glm::vec3(1, 1, 1)
	);

	void reCalculateObjectMatrix();

	void createRenderObject(char* meshName, char* materialName = "defaultMaterial", char* textureName = "defaultTexture");

	void createRigidBody_Box(float density = 1.0f);
	void createRigidBody_Cylinder(float density = 1.0f);
	void createRigidBody_Sphere(float density = 1.0f);

private:


};