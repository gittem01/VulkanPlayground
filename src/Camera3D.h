#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include "WindowHandler.h"

#define BASE_MULT 0.25f

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

	float zoom = 1;
	float zoomAim = 0.0f;

	float posSmth = 0.1f;
	float rotSmth = 0.1f;
	float zoomSmth = 0.1f;

	SDL_Window* window;

	float freeSpeed = 0.1f;
	float speedMult = BASE_MULT;

	int w, h;
	WindowHandler* wp;

	glm::vec2 zoomLimits = glm::vec2(0.5, 3);

	glm::mat4 pers;
	glm::mat4 view;

	glm::vec3 lookDir = glm::vec3(0, 0, -1);
	glm::vec3 topVec = glm::vec3(0, 1, 0);
	glm::vec3 rightVec;

	float dist = 3.0f;
	float distLimits[2] = { 1.0f, 50.0f };

	CameraTypes cameraType = SURROUNDER;

	Camera3D(glm::vec3 pos, WindowHandler* wp);
	void update();

	glm::mat4 getPers(int width, int height);
	glm::mat4 getView(bool posIncl);
	void changeZoom(float inc);
	float limitZoom(float inZoom);
	void rotateFunc();
	void keyControl();

	glm::vec3 rotatePoint(glm::vec3 point, glm::vec3 rotAngles);
	glm::vec3 rotatePointArround(glm::vec3 point, glm::vec3 arroundPoint, glm::vec3 rotAngles);
	void updateLookDir();
	void updatePos();
	void updateZoom();
	void updateWlkrPos();
	void controlRotation(glm::vec3* rot);

	glm::vec3 getVectorAngle(glm::vec3 vec);
};

