#include <vk_engine.h>

VulkanEngine* engine;
btDiscreteDynamicsWorld* world;

int rayMasks = btCollisionObject::CF_DYNAMIC_OBJECT | btCollisionObject::CF_KINEMATIC_OBJECT;

float getRand01() {
	return ((double)rand()) / RAND_MAX;
}

void createObjects(){
	GameObject* g = new GameObject(engine,
		glm::vec3(0.0f, -10.0f, 0.0f),
		glm::vec3(0.0f),
		glm::vec3(50.0f, 1.0f, 50.0f));
	g->createRenderObject("box");
	g->createRigidBody(0.0f);

	int n = 200;
	for (int i = 0; i < n; i++) {
		g = new GameObject(engine,
			glm::vec3((getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f),
			glm::vec3(0.0f),
			glm::vec3(0.5f));
		g->createRenderObject("box");
		g->createRigidBody(1.0f);
	}
}

void createPhysicsWorld() {
	///collision configuration contains default setup for memory, collision setup
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	
	btDbvtBroadphase* broadphase = new btDbvtBroadphase();
	
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, sol, collisionConfiguration);

	world->setGravity(btVector3(0, -10, 0));
}

void handleRays() {
	btCollisionWorld::ClosestRayResultCallback rayRes = engine->camera->rayToMouse();
	if (rayRes.hasHit()) {
		bool flags = (rayRes.m_collisionObject->getCollisionFlags() & rayMasks) ||
			(rayRes.m_collisionObject->getCollisionFlags() == 0);
		if (flags) {
			GameObject* object = (GameObject*)rayRes.m_collisionObject->getCollisionShape()->getUserPointer();
			rayRes.m_collisionObject->getCollisionFlags();
			object->renderObject->color.z = 1.0f;
		}
	}
}


int main(int argc, char* argv[]){
	createPhysicsWorld();

	engine = new VulkanEngine(1300, 700, world);
	
	engine->get_mesh("../../assets/monkey_flat.obj", "monkey");
	engine->get_mesh("../../assets/box.obj", "box");

	engine->get_image("../../assets/monkey.png", "monkey");
	engine->get_image("../../assets/defaultTexture.png", "defaultTexture");

	
	createObjects();

	bool done = false;
	while (!done) {
		done = engine->looper();

		if (engine->io->KeysDown[SDL_SCANCODE_SPACE]) {
			for (int i = 0; i < world->getNonStaticRigidBodies().size(); i++) {
				btRigidBody* rb = world->getNonStaticRigidBodies().at(i);
				rb->activate();
				btVector3 force = -rb->getCenterOfMassPosition();
				rb->applyCentralImpulse(force * rb->getMass() * 0.05f);
			}
		}

		world->stepSimulation(engine->io->DeltaTime);

		if (engine->io->MouseDownDuration[0] == 0.0f) {
			handleRays();
		}
	}

	return 0;
}