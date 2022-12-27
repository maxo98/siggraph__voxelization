#pragma once

#include <gtx/quaternion.hpp>

class Camera
{
public:
	Camera(glm::vec3 _pos, glm::quat _camRot, float fov, int height, int width);


	float wdithStep, heightStep, angle, aspectratio;
	float fov;
	glm::quat camRot;
	glm::vec3 pos;
};

