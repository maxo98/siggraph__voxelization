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
		Octree<glm::vec3>* map;
		int width, height, depth;
	};

	struct MapEntrance {
		glm::ivec3 normal;
		glm::vec3 pos;
	};

	VoxelScene();
	~VoxelScene();

	inline void addPointLight(const glm::vec3& light) { pointLights.push_back(light); }
	bool traceRay(VoxelMap& map, const glm::vec3& rayDir, const glm::vec3& pos, glm::vec3& color, glm::vec3& hitPos,
		glm::vec3& normal, glm::vec3* destination = nullptr, double lvl = 1, MapEntrance* entrance = nullptr);
	
	void drawPixels(int workload, int x, int y, Window& window, Camera& camera, std::vector<std::vector<glm::vec3>>& buffer, std::atomic<bool>* ticket = nullptr);

	void intersectPlane(const glm::vec3& normal, const glm::vec3& posPlane, const glm::vec3& posRay, const glm::vec3& rayDir, float& t);


	//Octree traveral
	uint8_t firstNode(glm::dvec3 t0, glm::dvec3 tm);
	uint8_t newNode(float txm, uint8_t x, float tym, uint8_t y, float tzm, uint8_t z);
	void newNormal(uint8_t oldNode, uint8_t newNode, glm::vec3& normal);
	bool rayParam(Octree<glm::vec3>* oct, const glm::vec3& octPos, glm::vec3 rayDir, glm::vec3 pos, double lvl, glm::vec3* color, glm::vec3& normal);
	bool procSubtree(glm::dvec3 t0, glm::dvec3 t1, Octree<glm::vec3>* octree, uint8_t a, glm::vec3* color, glm::vec3& normal);

private:
	VoxelMap worldMap;

	std::vector<glm::vec3> pointLights;
	uint8_t levels = 1;
	//float topLevelSize = 1; Assuming this will always be 1 simplifies things a little

	glm::vec3 lightColor = glm::vec3(1,1,1);
};