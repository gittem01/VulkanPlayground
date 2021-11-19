#include "vk_engine.h"

Camera3D::Camera3D(glm::vec3 pos, void* engine) {
	this->pos = pos;
	this->posAim = glm::vec3(0, 0, 0);

	this->rot = glm::vec3(0, 0, 0);
	this->rotAim = glm::vec3(0, 0, 0);

	this->zoom = 45.0f;
	this->zoomAim = 0.0f;

	this->engine = engine;

	this->rightVec = glm::normalize(glm::cross(topVec, lookDir));

	this->pers = this->getPers();

	lookDir = glm::vec3(0, 0, -1);
	topVec = glm::vec3(0, 1, 0);
	rightVec = glm::vec3(1, 0, 0);;
	realTopVec = topVec;

	strings[0] = (char*)"camera keyboard movement speed";
	strings[1] = (char*)"camera rotation speed";
	strings[2] = (char*)"camera wheel movement speed";
	strings[3] = (char*)"camera zoom speed";

	values[0] = &keyPosSpeed;
	values[1] = &rotSpeed;
	values[2] = &wheelPosSpeed;
	values[3] = &zoomSpeed;

	baseValues[0] = &keyPosSpeed_b;
	baseValues[1] = &rotSpeed_b;
	baseValues[2] = &wheelPosSpeed_b;
	baseValues[3] = &zoomSpeed_b;
}

glm::mat4 Camera3D::getPers()
{
	return glm::perspective(glm::radians(zoom), 
		(float)((VulkanEngine*)engine)->winExtent.width / (float)((VulkanEngine*)engine)->winExtent.height, 0.1f, 500.0f);
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
	VulkanEngine* egn = reinterpret_cast<VulkanEngine*>(engine);

	if (cameraType == WALKER) {
		if (!isAnyWindowHovered && egn->io->MouseWheel != 0) {
			zoomAim -= egn->io->MouseWheel * zoomSpeed * 0.4;
		}
		float diff = zoomAim * (zoomSmth * egn->io->DeltaTime * (float)enableZoomSmth + (1-enableZoomSmth));
		changeZoom(diff);
		zoomAim -= diff;
	}
	else if (cameraType == SURROUNDER) {
		if (!isAnyWindowHovered && egn->io->MouseWheel != 0) {
			posAim += lookDir * egn->io->MouseWheel * speedMult * wheelPosSpeed * 0.2f;
		}
		glm::vec3 diff = posAim * (wheelPosSmth * egn->io->DeltaTime * (float)enableWheelPosSmth + (1 - enableWheelPosSmth));
		pos += diff;
		posAim -= diff;
	}
}

void Camera3D::calculateTopRight() {
	rightVec = glm::normalize(glm::cross(topVec, lookDir));
	realTopVec = glm::normalize(glm::cross(lookDir, rightVec));
}

void Camera3D::update()
{
	VulkanEngine* egn = reinterpret_cast<VulkanEngine*>(engine);

	speedMult = BASE_MULT;

	if (egn->window) {
		updatePos();

		updateZoom();

		rotateFunc();
	}
	
	pers = getPers();
	view = getView(true);

	pers[1][1] *= -1;

	if (egn->io && egn->io->KeysDownDuration[SDL_SCANCODE_TAB] == 0.0f) {
		if (cameraType == SURROUNDER)
			cameraType = WALKER;
		else if (cameraType == WALKER)
			cameraType = SURROUNDER;
	}

	isAnyWindowHovered = false;
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
	VulkanEngine* egn = reinterpret_cast<VulkanEngine*>(engine);

	glm::vec3 lookDirection = glm::vec3(0, 0, -1);
	lookDir = rotatePoint(lookDirection, this->rot);

	if (cameraType == SURROUNDER) {
		if (!egn->io->MouseDownOwned[2] && egn->io->MouseDown[2] && egn->io->KeysDown[SDL_SCANCODE_LALT]) {
			posAim += rightVec * (egn->io->MouseDelta.x * 0.01f);
			posAim += realTopVec * (egn->io->MouseDelta.y * 0.01f);
		}
		glm::vec3 diff = posAim * (wheelPosSmth * egn->io->DeltaTime * (float)enableWheelPosSmth + (1 - enableWheelPosSmth));
		pos += diff;
		posAim -= diff;
	}
}

void Camera3D::rotateFunc()
{
	VulkanEngine* egn = reinterpret_cast<VulkanEngine*>(engine);

	if (!egn->io->MouseDownOwned[2] && egn->io->MouseDown[2] && (!egn->io->KeysDown[SDL_SCANCODE_LALT] || cameraType == WALKER)) {
		rotAim.x += egn->io->MouseDelta.y * rotSpeed * 0.0002f;
		rotAim.y += egn->io->MouseDelta.x * rotSpeed * 0.0002f;
	}

	glm::vec3 diff = rotAim * (rotSmth * egn->io->DeltaTime * (float)enableRotSmth + (1 - enableRotSmth));
	rot += diff;
	rotAim -= diff;
	controlRotation(&rot);
}

void Camera3D::updatePos()
{
	updateLookDir();
	calculateTopRight();
	keyControl();
}

void Camera3D::keyControl() {
	VulkanEngine* egn = reinterpret_cast<VulkanEngine*>(engine);

	if (egn->io->KeysDown[SDL_SCANCODE_LCTRL]) {
		speedMult *= 5.0f;
	}
	if (egn->io->KeysDown[SDL_SCANCODE_LSHIFT]) {
		speedMult *= 0.2f;
	}

	if (cameraType == WALKER) {
		updateWlkrPos();
	}
}

void Camera3D::updateWlkrPos() {	
	VulkanEngine* egn = reinterpret_cast<VulkanEngine*>(engine);

	glm::vec3 speedAddition = glm::vec3(0, 0, 0);

	if (egn->io->KeysDown[SDL_SCANCODE_W]) {
		speedAddition.x += keyPosSpeed * lookDir.x;
		speedAddition.y += keyPosSpeed * lookDir.y;
		speedAddition.z += keyPosSpeed * lookDir.z;
	}

	if (egn->io->KeysDown[SDL_SCANCODE_S]) {
		speedAddition.x -= keyPosSpeed * lookDir.x;
		speedAddition.y -= keyPosSpeed * lookDir.y;
		speedAddition.z -= keyPosSpeed * lookDir.z;
	}

	if (egn->io->KeysDown[SDL_SCANCODE_A]) {
		speedAddition.x += keyPosSpeed * rightVec.x;
		speedAddition.y += keyPosSpeed * rightVec.y;
		speedAddition.z += keyPosSpeed * rightVec.z;
	}

	if (egn->io->KeysDown[SDL_SCANCODE_D]) {
		speedAddition.x -= keyPosSpeed * rightVec.x;
		speedAddition.y -= keyPosSpeed * rightVec.y;
		speedAddition.z -= keyPosSpeed * rightVec.z;
	}

	if (egn->io->KeysDown[SDL_SCANCODE_Q]) {
		speedAddition.y += keyPosSpeed;
	}
	if (egn->io->KeysDown[SDL_SCANCODE_E]) {
		speedAddition.y -= keyPosSpeed;
	}

	posAim += speedAddition * speedMult * egn->io->DeltaTime;
	glm::vec3 diff = posAim * (keyPosSmth * egn->io->DeltaTime * (float)enableKeyPosSmth + (1 - enableKeyPosSmth));
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

glm::vec3 Camera3D::getRayDir(int x, int y) {
	VulkanEngine* egn = reinterpret_cast<VulkanEngine*>(engine);

	// excluding camera position to get only the direction
	glm::mat4 posExcldView = getView(false);
	glm::vec4 viewport = glm::vec4(0, 0, egn->winExtent.width / egn->dpiScaling, egn->winExtent.height / egn->dpiScaling);
	glm::vec3 screenPos = glm::vec3(x, y, 1.0f);
	glm::vec3 aimDir = glm::normalize(glm::unProject(screenPos, posExcldView, pers, viewport));

	return aimDir;
}

btCollisionWorld::ClosestRayResultCallback Camera3D::rayToMouse(btDynamicsWorld* dynamicsWorld) {
	return rayToPosition(glm::vec2	(	reinterpret_cast<VulkanEngine*>(engine)->io->MousePos.x, 
										reinterpret_cast<VulkanEngine*>(engine)->io->MousePos.y),	
										dynamicsWorld
						);
}

btCollisionWorld::ClosestRayResultCallback Camera3D::rayToPosition(glm::vec2 rayPos, btDynamicsWorld* dynamicsWorld) {
	VulkanEngine* egn = reinterpret_cast<VulkanEngine*>(engine);

	glm::vec3 aimDir = getRayDir((int)rayPos.x, (int)rayPos.y);

	btVector3 from(	pos.x, pos.y, pos.z);
	btVector3 to(	pos.x + aimDir.x * 1000.0f,
					pos.y + aimDir.y * 1000.0f,
					pos.z + aimDir.z * 1000.0f
	);

	btCollisionWorld::ClosestRayResultCallback closestResult(from, to);

	dynamicsWorld->rayTest(from, to, closestResult);

	return closestResult;
}

btCollisionWorld::ClosestRayResultCallback Camera3D::rayToCenter(btDynamicsWorld* dynamicsWorld) {
	VulkanEngine* egn = reinterpret_cast<VulkanEngine*>(engine);
	
	btVector3 from(pos.x, pos.y, pos.z);
	btVector3 to(	pos.x + lookDir.x * 10000.0f,
					pos.y + lookDir.y * 10000.0f,
					pos.z + lookDir.z * 10000.0f
	);

	btCollisionWorld::ClosestRayResultCallback closestResult(from, to);

	dynamicsWorld->rayTest(from, to, closestResult);

	return closestResult;
}