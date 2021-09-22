#include "Camera3D.h"

Camera3D::Camera3D(glm::vec3 pos, WindowHandler* wp) {
	this->pos = pos;
	this->posAim = glm::vec3(0, 0, 0);

	this->rot = glm::vec3(0, 0, 0);
	this->rotAim = glm::vec3(0, 0, 0);

	this->zoom = 45.0f;
	this->zoomAim = 0.0f;

	this->wp = wp;
	this->window = wp->window;

	this->rightVec = glm::normalize(glm::cross(topVec, lookDir));

	this->pers = this->getPers();
}


glm::mat4 Camera3D::getPers()
{
	return glm::perspective(glm::radians(zoom), (float)wp->winExtent.width / (float)wp->winExtent.height, 0.001f, 500.0f);
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
		if (wp->io->MouseWheel != 0) {
			zoomAim -= wp->io->MouseWheel * 2.5f;
		}
		float diff = zoomAim * zoomSmth * wp->io->DeltaTime;
		changeZoom(diff);
		zoomAim -= diff;
	}
	else if (cameraType == SURROUNDER) {
		if (wp->io->MouseWheel != 0) {			
			posAim += lookDir * wp->io->MouseWheel * speedMult;
		}
		glm::vec3 diff = posAim * wheelPosSmth * wp->io->DeltaTime;
		pos += diff;
		posAim -= diff;
	}
}

void Camera3D::update()
{
	speedMult = BASE_MULT;

	if (wp->window) {
		int width, height;
		SDL_GetWindowSize(this->window, &width, &height);

		updatePos();

		updateZoom();

		rotateFunc();
	}

	rightVec = glm::normalize(glm::cross(topVec, lookDir));
	
	pers = getPers();
	view = getView(true);

	pers[1][1] *= -1;

	if (wp->io && wp->io->KeysDownDuration[SDL_SCANCODE_TAB] == 0.0f) {
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
		if (wp->io->MouseDown[2] && wp->io->KeysDown[SDL_SCANCODE_LSHIFT]) {
			glm::vec3 yVec = glm::normalize(glm::cross(lookDir, rightVec));

			posAim += rightVec * (wp->io->MouseDelta.y * 0.01f);
			posAim += yVec * (wp->io->MouseDelta.x * 0.01f);
		}
		glm::vec3 diff = posAim * wheelPosSmth * wp->io->DeltaTime;
		pos += diff;
		posAim -= diff;
	}
}

void Camera3D::rotateFunc()
{
	if (wp->io->MouseDown[2] && (!wp->io->KeysDown[SDL_SCANCODE_LSHIFT] || cameraType == WALKER)) {
		rotAim.x += wp->io->MouseDelta.y * 0.002f;
		rotAim.y += wp->io->MouseDelta.x * 0.002f;
	}

	glm::vec3 diff = rotAim * rotSmth * wp->io->DeltaTime;
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
	if (wp->io->KeysDown[SDL_SCANCODE_LCTRL]) {
		speedMult *= 5.0f;
	}
	if (wp->io->KeysDown[SDL_SCANCODE_LSHIFT]) {
		speedMult *= 0.2f;
	}

	if (cameraType == WALKER) {
		updateWlkrPos();
	}
}

void Camera3D::updateWlkrPos()
{	
	glm::vec3 speedAddition = glm::vec3(0, 0, 0);

	if (wp->io->KeysDown[SDL_SCANCODE_W]) {
		speedAddition.x += freeSpeed * lookDir.x;
		speedAddition.y += freeSpeed * lookDir.y;
		speedAddition.z += freeSpeed * lookDir.z;
	}

	if (wp->io->KeysDown[SDL_SCANCODE_S]) {
		speedAddition.x -= freeSpeed * lookDir.x;
		speedAddition.y -= freeSpeed * lookDir.y;
		speedAddition.z -= freeSpeed * lookDir.z;
	}

	if (wp->io->KeysDown[SDL_SCANCODE_A]) {
		speedAddition.x += freeSpeed * rightVec.x;
		speedAddition.y += freeSpeed * rightVec.y;
		speedAddition.z += freeSpeed * rightVec.z;
	}

	if (wp->io->KeysDown[SDL_SCANCODE_D]) {
		speedAddition.x -= freeSpeed * rightVec.x;
		speedAddition.y -= freeSpeed * rightVec.y;
		speedAddition.z -= freeSpeed * rightVec.z;
	}

	if (wp->io->KeysDown[SDL_SCANCODE_Q]) {
		speedAddition.y += freeSpeed;
	}
	if (wp->io->KeysDown[SDL_SCANCODE_E]) {
		speedAddition.y -= freeSpeed;
	}

	posAim += speedAddition * speedMult * wp->io->DeltaTime;
	glm::vec3 diff = posAim * keyPosSmth * wp->io->DeltaTime;
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
