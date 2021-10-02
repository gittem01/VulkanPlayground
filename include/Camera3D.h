#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "btBulletDynamicsCommon.h"
#include <math.h>

#define BASE_MULT 0.25f
#define NUM_VALUES 4

typedef enum CameraTypes {
	WALKER = 1,
	SURROUNDER = 2,
}CameraTypes;

class Camera3D
{
public:
	glm::vec3 pos;
	glm::vec3 posAim;

	glm::vec3 rot;
	glm::vec3 rotAim;

	float zoom;
	float zoomAim;

	float keyPosSpeed = 10.0f;
	float wheelPosSpeed = 10.0f;
	float rotSpeed = 10.0f;
	float zoomSpeed = 10.0f;

	// base speeds
	float keyPosSpeed_b = 10.0f;
	float wheelPosSpeed_b = 10.0f;
	float rotSpeed_b = 10.0f;
	float zoomSpeed_b = 10.0f;

	char* strings[NUM_VALUES];
	float* values[NUM_VALUES];
	float* baseValues[NUM_VALUES];

	float keyPosSmth = 10.0f;
	float wheelPosSmth = 5.0f;
	float rotSmth = 5.0f;
	float zoomSmth = 5.0f;

	bool enableKeyPosSmth = true;
	bool enableWheelPosSmth = true;
	bool enableRotSmth = true;
	bool enableZoomSmth = true;

	bool isAnyWindowHovered = false;

	float speedMult = BASE_MULT;

	void* engine;

	glm::vec2 zoomLimits = glm::vec2(15.0f, 90.0f);

	glm::mat4 pers;
	glm::mat4 view;

	glm::vec3 lookDir;
	glm::vec3 topVec;
	glm::vec3 realTopVec;
	glm::vec3 rightVec;

	CameraTypes cameraType = SURROUNDER;

	Camera3D(glm::vec3 pos, void* engine);
	void update();

	glm::mat4 getPers();
	glm::mat4 getView(bool posIncl);

	void changeZoom(float inc);
	float limitZoom(float inZoom);
	void rotateFunc();
	void keyControl();

	// calculates the top and right vector according to lookDir
	void calculateTopRight();
	
	void updateLookDir();
	void updatePos();
	void updateZoom();
	void updateWlkrPos();
	void controlRotation(glm::vec3* rot);

	glm::vec3 getVectorAngle(glm::vec3 vec);
	glm::vec3 rotatePoint(glm::vec3 point, glm::vec3 rotAngles);
	glm::vec3 rotatePointArround(glm::vec3 point, glm::vec3 arroundPoint, glm::vec3 rotAngles);

	btCollisionWorld::ClosestRayResultCallback& rayToCenter();

	// rays to anywhere on the screen
	btCollisionWorld::ClosestRayResultCallback& rayToPosition(glm::vec2 rayPos);
	btCollisionWorld::ClosestRayResultCallback& rayToMouse();
};

