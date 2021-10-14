#pragma once

#include "vk_engine.h"

class PhysicsWorld {
public:
	VulkanEngine* engine;
	btDiscreteDynamicsWorld* world;

	btPoint2PointConstraint* mouseJoint;
	GameObject* clickedObject;
	float clickDist;

	int rayMasks;

	PhysicsWorld(VulkanEngine* engine);

	void createPhysicsWorld(btVector3& gravity);
	void mouseHandle();
	void loop();
};