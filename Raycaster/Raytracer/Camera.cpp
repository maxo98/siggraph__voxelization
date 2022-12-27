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