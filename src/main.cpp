#include <vk_engine.h>


float getRand01() {
	return ((double)rand()) / RAND_MAX;
}

void createObjects(VulkanEngine& engine, btDiscreteDynamicsWorld* dynamicsWorld){
	GameObject* g = new GameObject(&engine, dynamicsWorld,
		glm::vec3(0.0f, -10.0f, 0.0f),
		glm::vec3(0.0f),
		glm::vec3(50.0f, 1.0f, 50.0f));
	g->createRenderObject("box");
	g->createRigidBody(0.0f);

	int n = 200;
	for (int i = 0; i < n; i++) {
		g = new GameObject(&engine, dynamicsWorld,
			glm::vec3((getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f),
			glm::vec3(0.0f),
			glm::vec3(0.5f));
		g->createRenderObject("box");
		g->createRigidBody(1.0f);
	}
}

btDiscreteDynamicsWorld* createPhysicsWorld() {
	///collision configuration contains default setup for memory, collision setup
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	
	btDbvtBroadphase* broadphase = new btDbvtBroadphase();
	
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, sol, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	return dynamicsWorld;
}

int main(int argc, char* argv[]){
	VulkanEngine engine(1300, 700);
	
	engine.get_mesh("../../assets/monkey_flat.obj", "monkey");
	engine.get_mesh("../../assets/box.obj", "box");

	engine.get_image("../../assets/monkey.png", "monkey");
	engine.get_image("../../assets/defaultTexture.png", "defaultTexture");

	btDiscreteDynamicsWorld* world = createPhysicsWorld();
	
	createObjects(engine, world);

	bool done = false;
	while (!done) {
		done = engine.looper();

		if (engine.io->KeysDown[SDL_SCANCODE_SPACE]) {
			for (int i = 0; i < world->getNonStaticRigidBodies().size(); i++) {
				btRigidBody* rb = world->getNonStaticRigidBodies().at(i);
				rb->activate();
				btVector3 force = -rb->getCenterOfMassPosition();
				rb->applyCentralImpulse(force * rb->getMass() * 0.05f);
			}
		}

		world->stepSimulation(engine.io->DeltaTime);
	}

	return 0;
}