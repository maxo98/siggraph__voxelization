#pragma once

#include <math.h>
#include <glm.hpp>
#include "Octree.h"
#include "Window.h"
#include "Camera.h"
#include <mutex>
#include "Utils.h"

#define MAP_WIDTH 20
#define MAP_HEIGHT 20
#define MAP_DEPTH 20

class VoxelScene
{
public:
	enum class Axis{ X = 0, Y = 1, Z = 2 };

	struct VoxelMap {
		Octree<Color>* map;
		int width, height, depth;
	};

	struct MapEntrance {
		glm::ivec3 step;
		glm::vec3 pos;
	};

	VoxelScene();
	~VoxelScene();

	inline void addPointLight(const glm::uvec3& light) { pointLights.push_back(light); }
	bool traceRay(VoxelMap& map, const glm::vec3& rayDir, const glm::vec3& pos, Color& color, glm::vec3& hitPos,
		glm::vec3& normal, glm::vec3* destination = nullptr, float lvl = 1, MapEntrance* entrance = nullptr);
	
	void drawPixels(int workload, int x, int y, Window& window, Camera& camera, std::vector<std::vector<Color>>& buffer, std::atomic<bool>* ticket = nullptr);

private:
	VoxelMap worldMap;

	std::vector<glm::vec3> pointLights;
	uint8_t levels = 1;
	//float topLevelSize = 1; Assuming this will always be 1 simplifies things a little
};

