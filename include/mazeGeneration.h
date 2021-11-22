#include "GameObject.h"

#include <string.h>
#include <iostream>
#include <fstream>

void putBoxyAt(float mazeHeight, glm::vec2 scale, glm::vec3 pos, PhysicsWorld* world) {
    pos.y += mazeHeight / 2.0f;
    GameObject* g = new GameObject(world, pos, glm::vec3(), glm::vec3(0.5f * scale.x, mazeHeight / 2.0f, 1.0f * scale.y));

	g->createRenderObject("box");
    g->renderObject->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	g->createRigidBody_Box(0.0f);
}

void generateMaze(std::string mazeFileName, PhysicsWorld* world) {
    GameObject* g = new GameObject(world, glm::vec3(0, -13, 0), glm::vec3(), glm::vec3(300.0f, 3.0f, 300.0f));
	g->createRenderObject("box");
	g->createRigidBody_Box(0.0f);

    std::ifstream file = std::ifstream(mazeFileName);
    std::string line;
    float mazeScaleX = 0.5f;
    float mazeScaleZ = 0.25f;
    float x = 0;
    float z;
    while (getline (file, line)) {
        z = -1;
        for (int i = 0; i < line.size(); i++){
            if (line.at(i) == '#') putBoxyAt(0.3f, glm::vec2(mazeScaleX, mazeScaleZ), glm::vec3(x, -10, -z), world);
            z += mazeScaleZ;
        }
        x += mazeScaleX;
    }
}

void generateMazeVehicle(float scale, PhysicsWorld* world) {
    GameObject* vehicleBody = new GameObject(world, glm::vec3(0, -9.0f, 0), glm::vec3(), glm::vec3(1.0f, 0.1f, 1.0f) * scale);
	vehicleBody->createRenderObject("box");
	vehicleBody->createRigidBody_Box(1.0f);

    GameObject* wheel1 = new GameObject(world,
		glm::vec3(-0.6f * scale, -9.0f - 0.15f * scale, +(1.0f + 0.15f + 0.025f) * scale),
		glm::vec3(glm::pi<float>() / 2.0f, 0.0f, 0.0f),
		glm::vec3(0.3f, 0.15f, 0.3f) * scale);
	wheel1->createRenderObject("cylinder");
	wheel1->createRigidBody_Cylinder(1.0f);

    GameObject* wheel2 = new GameObject(world,
		glm::vec3(-0.6f * scale, -9.0f - 0.15f * scale, -(1.0f + 0.15f + 0.025f) * scale),
		glm::vec3(glm::pi<float>() / 2.0f, 0.0f, 0.0f),
		glm::vec3(0.3f, 0.15f, 0.3f) * scale);
	wheel2->createRenderObject("cylinder");
	wheel2->createRigidBody_Cylinder(1.0f);

    GameObject* ball = new GameObject(world,
		glm::vec3(0.8f * scale, -9.0f - (0.2f + 0.05f) * scale, 0.0f),
		glm::vec3(0.0f),
		glm::vec3(0.2f) * scale);
	ball->createRenderObject("sphere");
	ball->createRigidBody_Sphere(1.0f);


    btVector3 pivotInA = ball->rigidBody->getWorldTransform().getOrigin() + btVector3(0, 9, 0);
    btVector3 pivotInB(0, 0, 0);
    btVector3 axisInA(0, 0, 1);
    btVector3 axisInB(0, 1, 0);
    btVector3 mAxis = btVector3(0, 0, 1);
    bool useReferenceA = true;
    btHingeConstraint* hinge1 = new btHingeConstraint(   *vehicleBody->rigidBody, *ball->rigidBody,
                                                        pivotInA, pivotInB,
                                                        axisInA, axisInB, useReferenceA);

    pivotInA = wheel1->rigidBody->getWorldTransform().getOrigin() + btVector3(0, 9, 0);
    btHingeConstraint* hinge2 = new btHingeConstraint(  *vehicleBody->rigidBody, *wheel1->rigidBody,
                                                        pivotInA, pivotInB,
                                                        axisInA, axisInB, useReferenceA);

    pivotInA = wheel2->rigidBody->getWorldTransform().getOrigin() + btVector3(0, 9, 0);
    btHingeConstraint* hinge3 = new btHingeConstraint(  *vehicleBody->rigidBody, *wheel2->rigidBody,
                                                        pivotInA, pivotInB,
                                                        axisInA, axisInB, useReferenceA);

    world->world->addConstraint(hinge1, true);
    world->world->addConstraint(hinge2, true);
    world->world->addConstraint(hinge3, true);

    hinge2->enableMotor(true);
	hinge2->setMaxMotorImpulse(1000);
	hinge2->setMotorTargetVelocity(-7);
    
    hinge3->enableMotor(true);
	hinge3->setMaxMotorImpulse(1000);
	hinge3->setMotorTargetVelocity(-7);
}