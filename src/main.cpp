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
		glm::vec3(500.0f, 1.0f, 500.0f));
	g->createRenderObject("box", "defaultMaterial", "defaultTexture");
	g->createRigidBody_Box(0.0f);
	g->renderObject->color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

	float scaling = 4.0f;

	int n = 200;
	for (int i = 0; i < n; i++) {
		g = new GameObject(physicsWorld,
			glm::vec3((getRand01() - 0.5f), (getRand01() - 0.5f), (getRand01() - 0.5f)) * scaling * 5.0f,
			glm::vec3(0.0f),
			glm::vec3(getRand01() * 0.2f + 0.1f, getRand01() * 0.2f + 0.1f, getRand01() * 0.2f + 0.03f) * scaling);
		int randNum = rand() % 3;
		if (randNum == 0){
			g->createRenderObject("box");
			//g->createRigidBody_Box(1.0f);
		}
		else if (randNum == 1){
			g->createRenderObject("sphere_smooth");
			g->scl = glm::vec3(g->scl.x);
			//g->createRigidBody_Sphere(1.0f);
		}

		else if (randNum == 2){
			g->createRenderObject("cylinder");
			g->scl = glm::vec3(g->scl.x);
			//g->createRigidBody_Cylinder(1.0f);
		}
		
		g->renderObject->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

int main(int argc, char* argv[]){
	vkEngine = new VulkanEngine(1400, 750);

	physicsWorld = new PhysicsWorld(vkEngine);

	vkEngine->get_mesh("../../assets/monkey_flat.obj", "monkey");
	vkEngine->get_mesh("../../assets/box.obj", "box");
	vkEngine->get_mesh("../../assets/cylinder.obj", "cylinder");
	vkEngine->get_mesh("../../assets/sphere.obj", "sphere");
	vkEngine->get_mesh("../../assets/sphere_smooth.obj", "sphere_smooth");

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