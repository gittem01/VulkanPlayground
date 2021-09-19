#include "Camera3D.h"

Camera3D::Camera3D(glm::vec3 pos, WindowHandler* wp) {
	this->pos = pos;
	this->posAim = glm::vec3(0, 0, 0);

	this->rot = glm::vec3(0, 0, 0);
	this->rotAim = glm::vec3(0, 0, 0);

	this->wp = wp;
	if (wp) this->window = wp->window;
	else this->window = NULL;

	this->rightVec = glm::normalize(glm::cross(topVec, lookDir));

	int width, height;
	SDL_GetWindowSize(this->window, &width, &height);
	this->pers = this->getPers(width, height);
	this->w = width; this->h = height;
}


glm::mat4 Camera3D::getPers(int width, int height)
{
	return glm::perspective(glm::radians(45.0f / this->zoom), (float)width / (float)height, 0.001f, 500.0f);
}

glm::mat4 Camera3D::getView(bool posIncl)
{
	if (posIncl) {
		return glm::lookAt(this->pos, this->lookDir + this->pos, this->topVec);
	}
	else {
		return glm::lookAt(glm::vec3(0), this->lookDir, this->topVec);
	}
}

void Camera3D::changeZoom(float inc)
{
	zoom += inc;
	zoom = limitZoom(zoom);
}

float Camera3D::limitZoom(float inZoom)
{
	if (inZoom < zoomLimits.x) {
		inZoom = zoomLimits.x;
	}
	else if (inZoom > zoomLimits.y) {
		inZoom = zoomLimits.y;
	}
	return inZoom;
}

void Camera3D::updateZoom() {
	if (cameraType == WALKER) {
		if (wp->mouseData[5] != 0) {
			zoomAim += 0.1f * this->wp->mouseData[5];
		}
		float diff = zoomAim * zoomSmth;
		changeZoom(diff);
		zoomAim -= diff;
	}
	else if (cameraType == SURROUNDER) {
		if (wp->mouseData[5] != 0) {
			posAim += lookDir * (float)wp->mouseData[5] * speedMult;
		}
		glm::vec3 diff = posAim * posSmth;
		pos += diff;
		posAim -= diff;
	}
}

void Camera3D::update()
{
	speedMult = BASE_MULT;

	if (wp) {
		int width, height;
		SDL_GetWindowSize(this->window, &width, &height);
		this->w = width; this->h = height;

		updatePos();

		updateZoom();

		rotateFunc();
	}

	rightVec = glm::normalize(glm::cross(topVec, lookDir));
	
	pers = getPers(w, h);
	view = getView(true);

	pers[1][1] *= -1;

	if (wp && wp->keyData[SDL_SCANCODE_TAB] == 2) {
		if (cameraType == SURROUNDER)
			cameraType = WALKER;
		else if (cameraType == WALKER)
			cameraType = SURROUNDER;
	}
}

glm::vec3 Camera3D::rotatePoint(glm::vec3 point, glm::vec3 rotAngles)
{
	glm::mat4 rotator = glm::mat4(1.0f);

	rotator = glm::rotate(rotator, rotAngles.x, glm::vec3(1, 0, 0));
	rotator = glm::rotate(rotator, rotAngles.y, glm::vec3(0, 1, 0));
	rotator = glm::rotate(rotator, rotAngles.z, glm::vec3(0, 0, 1));

	glm::vec4 v = glm::vec4(point.x, point.y, point.z, 1.0f) * rotator;

	return glm::vec3(v);
}

glm::vec3 Camera3D::rotatePointArround(glm::vec3 point, glm::vec3 arroundPoint, glm::vec3 rotAngles)
{
	glm::vec3 diff = point - arroundPoint;
	glm::mat4 rotator = glm::mat4(1.0f);

	rotator = glm::rotate(rotator, rotAngles.x, glm::vec3(1, 0, 0));
	rotator = glm::rotate(rotator, rotAngles.y, glm::vec3(0, 1, 0));
	rotator = glm::rotate(rotator, rotAngles.z, glm::vec3(0, 0, 1));

	glm::vec4 v = glm::vec4(diff.x, diff.y, diff.z, 1.0f) * rotator;

	return glm::vec3(v) + arroundPoint;
}

void Camera3D::updateLookDir() {

	glm::vec3 lookDirection = glm::vec3(0, 0, -1);
	lookDir = rotatePoint(lookDirection, this->rot);
	
	if (cameraType == SURROUNDER) {
		if (wp->mouseData[3] && wp->keyData[SDL_SCANCODE_LSHIFT]) {
			glm::vec3 yVec = glm::normalize(glm::cross(lookDir, rightVec));

			posAim += rightVec * (wp->moveDiff[0] * 0.01f);
			posAim += yVec * (wp->moveDiff[1] * 0.01f);
		}
		glm::vec3 diff = posAim * posSmth;
		pos += diff;
		posAim -= diff;
	}
}

void Camera3D::rotateFunc()
{
	if (wp->mouseData[3] && (!wp->keyData[SDL_SCANCODE_LSHIFT] || cameraType == WALKER)) {
		rotAim.x += wp->moveDiff[1] * 0.002f;
		rotAim.y += wp->moveDiff[0] * 0.002f;
	}

	glm::vec3 diff = rotAim * rotSmth;
	rot += diff;
	rotAim -= diff;
	controlRotation(&rot);
}

void Camera3D::updatePos()
{
	updateLookDir();
	keyControl();
}

void Camera3D::keyControl() {
	if (wp->keyData[SDL_SCANCODE_LCTRL]) {
		speedMult *= 5.0f;
	}
	if (wp->keyData[SDL_SCANCODE_LSHIFT]) {
		speedMult *= 0.2f;
	}

	if (cameraType == WALKER) {
		updateWlkrPos();
	}
}

void Camera3D::updateWlkrPos()
{	
	
	glm::vec3 speedAddition = glm::vec3(0, 0, 0);

	if (wp->keyData[SDL_SCANCODE_W]) {
		speedAddition.x += freeSpeed * lookDir.x;
		speedAddition.y += freeSpeed * lookDir.y;
		speedAddition.z += freeSpeed * lookDir.z;
	}

	if (wp->keyData[SDL_SCANCODE_S]) {
		speedAddition.x -= freeSpeed * lookDir.x;
		speedAddition.y -= freeSpeed * lookDir.y;
		speedAddition.z -= freeSpeed * lookDir.z;
	}

	if (wp->keyData[SDL_SCANCODE_A]) {
		speedAddition.x += freeSpeed * rightVec.x;
		speedAddition.y += freeSpeed * rightVec.y;
		speedAddition.z += freeSpeed * rightVec.z;
	}

	if (wp->keyData[SDL_SCANCODE_D]) {
		speedAddition.x -= freeSpeed * rightVec.x;
		speedAddition.y -= freeSpeed * rightVec.y;
		speedAddition.z -= freeSpeed * rightVec.z;
	}

	if (wp->keyData[SDL_SCANCODE_Q]) {
		speedAddition.y += freeSpeed;
	}
	if (wp->keyData[SDL_SCANCODE_E]) {
		speedAddition.y -= freeSpeed;
	}

	posAim += speedAddition * speedMult;
	glm::vec3 diff = posAim * posSmth;
	pos += diff;
	posAim -= diff;
}

void Camera3D::controlRotation(glm::vec3* rot) {
	if (rot->x >= glm::radians(90.0f)) {
		rot->x = glm::pi<float>() / 2.001f;
	}
	else if (rot->x <= -glm::radians(90.0f)) {
		rot->x = -glm::pi<float>() / 2.001f;
	}
}

glm::vec3 Camera3D::getVectorAngle(glm::vec3 vec){
	vec = glm::normalize(vec);

	float angleY = asin(vec.x);
	float angleX = asin(vec.z)/cos(angleY);

	return glm::vec3(angleX, angleY, 0);
}
