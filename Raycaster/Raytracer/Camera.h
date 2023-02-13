#pragma once

#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>


class Camera
{
public:
	Camera(glm::vec3 _pos, glm::quat _camRot, float fov, int width, int height);


	float wdithStep, heightStep, angle, aspectratio;
	float fov;
	glm::quat camRot;
	glm::vec3 pos;
	int windowWidth, windowHeight;

	void rotateCamera(int x, int y, float speed);
	void lookAt(glm::vec3 center);
};