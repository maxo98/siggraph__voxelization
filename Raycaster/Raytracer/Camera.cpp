#include "Camera.h"

#include <SDL.h>

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
	float xDistanceFromWindowCenter = x - windowWidth / 2.f;
	float yDistanceFromWindowCenter = (windowHeight / 2.f) - y;
	float yaw = xDistanceFromWindowCenter * speed;
	float pitch = yDistanceFromWindowCenter * speed;
	glm::vec3 eulerAngles = glm::eulerAngles(camRot);
	eulerAngles.x = yaw;
	eulerAngles.y = pitch;
	eulerAngles.x = 0.f;
	camRot = glm::quat(eulerAngles);
}

