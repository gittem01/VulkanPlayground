#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld(VulkanEngine* engine) {
	this->engine = engine;

	this->world = NULL;
	this->mouseJoint = NULL;
	this->clickedObject = NULL;

	this->rayMasks = btCollisionObject::CF_DYNAMIC_OBJECT | btCollisionObject::CF_KINEMATIC_OBJECT;

	btVector3 v = btVector3(0, -9.8f, 0);
	createPhysicsWorld(v);
}

void PhysicsWorld::createPhysicsWorld(btVector3& gravity) {
	///collision configuration contains default setup for memory, collision setup
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	collisionConfiguration->setConvexConvexMultipointIterations();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	btDbvtBroadphase* broadphase = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;

	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, sol, collisionConfiguration);

	world->setGravity(gravity);
}

void PhysicsWorld::mouseHoldHandle() {
	btVector3 camBtPos = btVector3(engine->camera->pos.x, engine->camera->pos.y, engine->camera->pos.z);
	glm::vec3 rayDir = engine->camera->getRayDir((int)engine->io->MousePos.x, (int)engine->io->MousePos.y);
	btVector3 rayDirBt = btVector3(rayDir.x, rayDir.y, rayDir.z);

	btPoint2PointConstraint* pickConstr = static_cast<btPoint2PointConstraint*>(mouseJoint);
	if (pickConstr) {
		rayDirBt *= clickDist;
		pickConstr->setPivotB(rayDirBt + camBtPos);
	}
}

void PhysicsWorld::mouseClickHandle() {
	btCollisionWorld::ClosestRayResultCallback rayRes = engine->camera->rayToMouse(world);
	if (rayRes.hasHit()) {
		bool flags = (rayRes.m_collisionObject->getCollisionFlags() & rayMasks) || (rayRes.m_collisionObject->getCollisionFlags() == 0);
		if (flags) {
			clickedObject = (GameObject*)rayRes.m_collisionObject->getCollisionShape()->getUserPointer();
			rayRes.m_collisionObject->getCollisionFlags();
			clickedObject->renderObject->color.z = 1.0f;

			btVector3 hitPoint = rayRes.m_hitPointWorld;

			btVector3 camBtPos = btVector3(engine->camera->pos.x, engine->camera->pos.y, engine->camera->pos.z);
			clickDist = (camBtPos - hitPoint).length();

			clickedObject->rigidBody->activate();
			btVector3 localPivot = clickedObject->rigidBody->getCenterOfMassTransform().inverse() * hitPoint;
			mouseJoint = new btPoint2PointConstraint(*clickedObject->rigidBody, localPivot);
			world->addConstraint(mouseJoint, true);

			mouseJoint->m_setting.m_impulseClamp = 30.f;
			mouseJoint->m_setting.m_tau = 0.001f;
		}
	}
}

void PhysicsWorld::loop() {
	if (engine->io->KeysDown[SDL_SCANCODE_SPACE]) {
		int multiplier = 1.0f;
		if (engine->io->KeysDown[SDL_SCANCODE_LSHIFT]) multiplier *= -1;
		for (int i = 0; i < world->getNonStaticRigidBodies().size(); i++) {
			btRigidBody* rb = world->getNonStaticRigidBodies().at(i);
			rb->activate();
			btVector3 force = -rb->getCenterOfMassPosition();
			rb->applyCentralImpulse(force * rb->getMass() * 0.05f * multiplier);
		}
	}

	world->stepSimulation(engine->io->DeltaTime);

	if (!engine->io->MouseDownOwned[0]) {
		if (engine->io->MouseDownDuration[0] == 0.0f) {
			mouseClickHandle();
		}
		else if (engine->io->MouseDownDuration[0] > 0.0f) {
			mouseHoldHandle();
		}
		
		else if (clickedObject && !engine->io->MouseDown[0]) {
			clickedObject->renderObject->color.z = 0.0f;

			if (mouseJoint)
			{
				clickedObject->rigidBody->activate();
				world->removeConstraint(mouseJoint);
				delete mouseJoint;
				mouseJoint = NULL;
				clickedObject = NULL;
			}
		}
	}

	if (clickedObject) {
		if (engine->io->KeysDownDuration[SDL_SCANCODE_Z] >= 0) {
			clickDist -= 20.0f * engine->io->DeltaTime;
			if (clickDist < 5.0f) clickDist = 5.0f;
		}
		if (engine->io->KeysDownDuration[SDL_SCANCODE_X] >= 0) {
			clickDist += 20.0f * engine->io->DeltaTime;
		}
		if (engine->io->MouseDownDuration[1] == 0.0f) {
			clickedObject->renderObject->color.z = 0.0f;
			clickedObject = NULL;
			mouseJoint = NULL;
		}
	}
}