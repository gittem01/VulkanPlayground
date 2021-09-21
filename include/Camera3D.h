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

	float zoom;
	float zoomAim;

	float keyPosSmth = 10.0f;
	float wheelPosSmth = 5.0f;
	float rotSmth = 5.0f;
	float zoomSmth = 5.0f;

	SDL_Window* window;

	float freeSpeed = 5.0f;
	float speedMult = BASE_MULT;

	int w, h;
	WindowHandler* wp;

	glm::vec2 zoomLimits = glm::vec2(15.0f, 90.0f);

	glm::mat4 pers;
	glm::mat4 view;

	glm::vec3 lookDir = glm::vec3(0, 0, -1);
	glm::vec3 topVec = glm::vec3(0, 1, 0);
	glm::vec3 rightVec = glm::vec3(1, 0, 0);;

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

