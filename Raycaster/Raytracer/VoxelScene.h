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
	bool traceRay(VoxelMap& map, const glm::vec3& rayDir, const glm::vec3& pos, Octree<glm::vec3>** octreeHit, glm::vec3& hitPos,
		glm::vec3& normal, float lvl = 1, MapEntrance* entrance = nullptr);

	void addPoint(glm::dvec3 pos, glm::vec3 color);
	void simplify();
	void simplifyOctree(Octree<glm::vec3>* tree);
	bool loadModel(glm::dvec3 pos, std::string file);
	
	void drawPixels(int workload, int x, int y, Window& window, Camera& camera, std::vector<std::vector<glm::vec3>>& buffer, std::atomic<bool>* ticket = nullptr);

	//Octree traveral
	uint8_t firstNode(glm::dvec3 t0, glm::dvec3 tm, float& t);
	uint8_t newNode(float txm, uint8_t x, float tym, uint8_t y, float tzm, uint8_t z, float& t);
	void newNormal(uint8_t oldNode, uint8_t newNode, glm::vec3& normal);
	bool rayParam(Octree<glm::vec3>* oct, const glm::vec3& octPos, glm::vec3 rayDir, glm::vec3 pos, float lvl, Octree<glm::vec3>** octreeHit, glm::vec3& normal, float& t, bool& hitOnEnter);
	bool procSubtree(glm::dvec3 t0, glm::dvec3 t1, Octree<glm::vec3>* octree, const glm::vec3& octPos,
		float lvl, uint8_t a, Octree<glm::vec3>** octreeHit, glm::vec3& normal, float& t, bool& hitOnEnter);

private:
	VoxelMap worldMap;

	std::vector<glm::vec3> pointLights;
	uint8_t levels = 7;
	//float topLevelSize = 1; Assuming this will always be 1 simplifies things a little

	glm::vec3 lightColor = glm::vec3(1.2, 1.2, 1.2);

	//glm::vec3 gridOffset = glm::vec3(10, 10, 10);
};