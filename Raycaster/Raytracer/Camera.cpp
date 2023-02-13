#include "Camera.h"

#include <SDL.h>
#include <iostream>


Camera::Camera(glm::vec3 _pos, glm::quat _camRot, float _fov, int width, int height)
{
	heightStep = 1 / (float)height;
	wdithStep = 1 / (float)width;
	fov = _fov, aspectratio = width / float(height);
	angle = tan(M_PI * 0.5 * fov / 180.);
	camRot = _camRot;
	pos = _pos;
	windowWidth = width;
	windowHeight = height;
}

void Camera::rotateCamera(int x, int y, float speed)
{
	float yaw = x * speed;
	float pitch = y * speed;
	glm::vec3 eulerAngles = glm::eulerAngles(camRot);
	eulerAngles.x = yaw;
	eulerAngles.y = pitch;
	eulerAngles.z = 0.f;
	camRot = glm::quat(eulerAngles);
}

void Camera::lookAt(glm::vec3 center)
{
	camRot = glm::conjugate(glm::toQuat(glm::lookAtLH(this->pos, center, glm::vec3(0, 1, 0))));
}
	

