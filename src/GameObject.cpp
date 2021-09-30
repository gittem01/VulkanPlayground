#include "GameObject.h"
#include "vk_engine.h"

GameObject::GameObject(void* engine, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale){

	this->engine = engine;

	this->renderObject = NULL;
	this->rigidBody = NULL;

	this->pos = position;
	this->rot = rotation;
	this->scl = scale;

	reCalculateObjectMatrix();

	((VulkanEngine*)engine)->gameObjects.push_back(this);
}


void GameObject::reCalculateObjectMatrix() {
	glm::mat4 translateMatrix = glm::mat4(1.0f);
	translateMatrix = glm::translate(translateMatrix, pos);

	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::vec3 v = glm::vec3();
	for (int i = 0; i < 3; i++) {
		v[i] = 1;
		rotationMatrix = glm::rotate(rotationMatrix, rot[i], v);
		v[i] = 0;
	}

	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scl);

	objectMatrix = translateMatrix * rotationMatrix * scaleMatrix;
}

void GameObject::createRenderObject(char* meshName, char* textureName, char* materialName) {
	renderObject = new RenderObject{ meshName, materialName, textureName,
		glm::vec4(((float)rand())/RAND_MAX, ((float)rand()) / RAND_MAX, ((float)rand()) / RAND_MAX, 1.0f)};
}