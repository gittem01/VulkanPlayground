#include "GameObject.h"
#include "PhysicsWorld.h"

GameObject::GameObject(void* physicsWorld,
	glm::vec3 position, glm::vec3 rotation, glm::vec3 scale){

	this->physicsWorld = physicsWorld;

	this->renderObject = NULL;
	this->rigidBody = NULL;

	this->pos = position;
	this->rot = rotation;
	this->scl = scale;

	reCalculateObjectMatrix();

	PhysicsWorld* pWorld = reinterpret_cast<PhysicsWorld*>(physicsWorld);

	(((VulkanEngine*)pWorld->engine))->gameObjects.push_back(this);
}


void GameObject::reCalculateObjectMatrix() {
	if (rigidBody) {
		btVector3 rbPos = rigidBody->getCenterOfMassPosition();

		pos.x = rbPos.getX();
		pos.y = rbPos.getY();
		pos.z = rbPos.getZ();

		if (rigidBody->getMass() > 0) {
			if (!rigidBody->isActive()) {
				renderObject->color.y = 0.0f;
			}
			else {
				renderObject->color.y = 1.0f;
			}
		}
	}
	
	glm::mat4 translateMatrix = glm::mat4(1.0f);
	translateMatrix = glm::translate(translateMatrix, pos);

	rotationMatrix = glm::mat4(1.0f);
	if (rigidBody) {
		btQuaternion quaternion = rigidBody->getCenterOfMassTransform().getRotation();
		rotationMatrix = glm::quat(quaternion.getW(), quaternion.getX(), quaternion.getY(), quaternion.getZ())
			.operator glm::mat<4, 4, glm::f32, glm::packed_highp>();
	}
	else {
		glm::vec3 v = glm::vec3();
		for (int i = 0; i < 3; i++) {
			v[i] = 1;
			rotationMatrix = glm::rotate(rotationMatrix, rot[i], v);
			v[i] = 0;
		}
	}
	
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scl);

	objectMatrix = translateMatrix * rotationMatrix * scaleMatrix;
}

void GameObject::createRenderObject(char* meshName, char* materialName, char* textureName) {
	renderObject = new RenderObject{ meshName, materialName, textureName,
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)};
}

void GameObject::createRigidBody_Box(float density) {
	PhysicsWorld* pWorld = reinterpret_cast<PhysicsWorld*>(physicsWorld);

	btBoxShape* bs = new btBoxShape(btVector3(scl.x, scl.y, scl.z));

	btVector3 inertia;
	bs->calculateLocalInertia(density, inertia);
	btDefaultMotionState* ms = new btDefaultMotionState();
	
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(pos.x, pos.y, pos.z));

	btQuaternion q;
	q.setEulerZYX(rot.z, rot.y, rot.x);

	t.setRotation(q);

	ms->setWorldTransform(t);

	rigidBody = new btRigidBody(density, ms, bs, inertia);
	rigidBody->setFriction(0.5f);
	rigidBody->setDamping(0.2f, 0.2f);

	pWorld->world->addRigidBody(rigidBody);

	bs->setUserPointer((void*)this);
}

void GameObject::createRigidBody_Cylinder(float density) {
	PhysicsWorld* pWorld = reinterpret_cast<PhysicsWorld*>(physicsWorld);

	btCylinderShape* sh = new btCylinderShape(btVector3(scl.x, scl.y, scl.z));

	btVector3 inertia;
	sh->calculateLocalInertia(density, inertia);
	btDefaultMotionState* ms = new btDefaultMotionState();
	
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(pos.x, pos.y, pos.z));

	btQuaternion q;
	q.setEulerZYX(rot.z, rot.y, rot.x);

	t.setRotation(q);

	ms->setWorldTransform(t);

	rigidBody = new btRigidBody(density, ms, sh, inertia);
	rigidBody->setFriction(1.0f);
	rigidBody->setDamping(0.0f, 0.0f);

	pWorld->world->addRigidBody(rigidBody);

	sh->setUserPointer((void*)this);
}

void GameObject::createRigidBody_Sphere(float density) {
	PhysicsWorld* pWorld = reinterpret_cast<PhysicsWorld*>(physicsWorld);

	btSphereShape* sh = new btSphereShape(scl.x);

	btVector3 inertia;
	sh->calculateLocalInertia(density, inertia);
	btDefaultMotionState* ms = new btDefaultMotionState();
	
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(pos.x, pos.y, pos.z));

	btQuaternion q;
	q.setEulerZYX(rot.z, rot.y, rot.x);

	t.setRotation(q);

	ms->setWorldTransform(t);

	rigidBody = new btRigidBody(density, ms, sh, inertia);
	rigidBody->setFriction(1.0f);
	rigidBody->setDamping(0.0f, 0.0f);

	pWorld->world->addRigidBody(rigidBody);

	sh->setUserPointer((void*)this);
}