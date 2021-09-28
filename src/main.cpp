#include <vk_engine.h>
#include "btBulletDynamicsCommon.h"

float getRand01() {
	return ((double)rand()) / RAND_MAX;
}

void createObjects(VulkanEngine& engine){
	int n = 400;
	for (int i = 0; i < n; i++) {
		RenderObject monkey;
		monkey.meshName = "monkey";
		monkey.textureName = "monkey";
		monkey.materialName = "defaultMaterial";
		glm::mat4 translation = glm::translate(glm::mat4{ 1.0 },
			glm::vec3((getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f));
		glm::mat4 scale = glm::scale(glm::mat4{ 1.0 },
			glm::vec3(0.5, 0.5, 0.5));
		monkey.transformMatrix = translation * scale;
		monkey.color = glm::vec4(getRand01(), getRand01(), getRand01(), 1.0f);
		engine._renderables.push_back(monkey);
	}
}

btDiscreteDynamicsWorld* createPhysicsWorld() {
	///collision configuration contains default setup for memory, collision setup
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	//m_collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	btDbvtBroadphase* broadphase = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, sol, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	btBoxShape* bs = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
	btDefaultMotionState* ms = new btDefaultMotionState();
	btRigidBody* body = new btRigidBody(1.0f, ms, bs);
	dynamicsWorld->addRigidBody(body);

	
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(0, -5, 0));
	btDefaultMotionState* ms2 = new btDefaultMotionState(t);
	btRigidBody* body2 = new btRigidBody(btScalar(0.), ms2, bs);
	dynamicsWorld->addRigidBody(body2);

	return dynamicsWorld;
}

int main(int argc, char* argv[]){
	VulkanEngine engine(1300, 700);
	
	engine.get_mesh("../../assets/monkey_flat.obj", "monkey");
	engine.get_image("../../assets/monkey.png", "monkey");

	createObjects(engine);
	btDiscreteDynamicsWorld* world = createPhysicsWorld();
	btRigidBody* body = world->getNonStaticRigidBodies().at(0);

	bool done = false;
	while (!done) {
		done = engine.looper();

		// 1x1x1x box starts at (0, 0, 0) and collides with the static box size of 1x1x1 which is at the (0, -5, 0)
		world->stepSimulation(engine.io->DeltaTime);
		btVector3 pos = body->getCenterOfMassPosition();
		printf("body position: %f, %f, %f\n", pos.getX(), pos.getY(), pos.getZ());
	}

	return 0;
}