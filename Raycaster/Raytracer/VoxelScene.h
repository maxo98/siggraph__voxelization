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

	inline void addPointLight(const glm::uvec3& light) { pointLights.push_back(light); }

	bool traceRay(const glm::vec3& rayDir, const glm::vec3& pos, Color& color, glm::uvec3& hitPos, glm::uvec3* destination = nullptr);
	bool traceRay(const glm::vec3& rayDir, const glm::uvec3& pos, Color& color, glm::uvec3& hitPos, glm::uvec3* destination = nullptr);
	void drawPixels(int workload, int x, int y, Window& window, Camera& camera, std::vector<std::vector<Color>>& buffer, std::atomic<bool>* ticket = nullptr);

private:

	Voxel worldMap[mapWidth][mapDepth][mapHeight] = {};
	std::vector<glm::uvec3> pointLights;
};

