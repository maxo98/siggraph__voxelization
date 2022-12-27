#pragma once

#include <math.h>
#include <glm.hpp>
#include "Voxel.h"
#include "Window.h"
#include "Camera.h"
#include <mutex>

#define mapWidth 24
#define mapHeight 24
#define mapDepth 3

class VoxelScene
{
public:
	VoxelScene();

	bool traceRay(glm::vec3 rayDir, glm::vec3 pos, Color& color);

	void drawPixel(int x, int y, Window& window, Camera& camera, std::vector<std::vector<Color>>& buffer, std::atomic<bool>* ticket = nullptr);

private:

	Voxel worldMap[mapWidth][mapDepth][mapHeight] = {};

	std::mutex mtx;
};

