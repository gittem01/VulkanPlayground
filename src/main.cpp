#include "PhysicsWorld.h"

VulkanEngine* vkEngine = NULL;
PhysicsWorld* physicsWorld = NULL;


double getRand01(){
	return ((double)rand()) / RAND_MAX;
}

void createObjects(){
	GameObject* g = new GameObject(physicsWorld,
		glm::vec3(0.0f, -10.0f, 0.0f),
		glm::vec3(0.0f),
		glm::vec3(50.0f, 1.0f, 50.0f));
	g->createRenderObject("box");
	g->createRigidBody(0.0f);

	int n = 100;
	for (int i = 0; i < n; i++) {
		g = new GameObject(physicsWorld,
			glm::vec3((getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f),
			glm::vec3(0.0f),
			glm::vec3(getRand01() + 0.3f, getRand01() + 0.3f, getRand01() + 0.3f));
		g->createRenderObject("box");
		g->createRigidBody(1.0f);
	}
}

int main(int argc, char* argv[]){
	vkEngine = new VulkanEngine(1400, 750);

	physicsWorld = new PhysicsWorld(vkEngine);

	vkEngine->get_mesh("../../assets/monkey_flat.obj", "monkey");
	vkEngine->get_mesh("../../assets/box.obj", "box");

	vkEngine->get_image("../../assets/monkey.png", "monkey");
	vkEngine->get_image("../../assets/defaultTexture.png", "defaultTexture");

	createObjects();

	bool done = false;
	while (!done) {
		done = vkEngine->looper();
		physicsWorld->loop();
	}

	return 0;
}