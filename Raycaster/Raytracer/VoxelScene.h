#pragma once

#include <math.h>
#include <glm.hpp>
#include "Octree.h"
#include "Window.h"
#include "Camera.h"
#include <mutex>
#include "Utils.h"
#include "Hyperneat.h"

#define MAP_WIDTH 20
#define MAP_HEIGHT 20
#define MAP_DEPTH 20

class VoxelScene
{
public:
	enum class Axis{ X = 0, Y = 1, Z = 2 };

	struct VoxelMap {
		Octree<glm::dvec3>* map;
		int width, height, depth;
	};

	struct MapEntrance {
		glm::ivec3 normal;
		glm::dvec3 pos;
	};

	VoxelScene(int _levels);
	~VoxelScene();

	inline void addPointLight(const glm::dvec3& light) { pointLights.push_back(light); }
	bool traceRay(VoxelMap& map, const glm::dvec3& rayDir, const glm::dvec3& pos, Octree<glm::dvec3>** octreeHit, glm::dvec3& hitPos,
		glm::dvec3& normal);

	bool addPoint(glm::dvec3 pos, glm::dvec3 color);
	void simplify();
	void simplifyOctree(Octree<glm::dvec3>* tree);
	bool loadModel(glm::dvec3 pos, std::string file);
	
	void drawPixels(int workload, int x, int y, Window& window, Camera& camera, std::vector<std::vector<glm::vec3>>& buffer, 
		double octSize, int radius, Hyperneat* hyperneat = nullptr, NeuralNetwork* network = nullptr, std::atomic<bool>* ticket = nullptr);

	//Octree traveral
	uint8_t firstNode(glm::dvec3 t0, glm::dvec3 tm, double& t);
	uint8_t newNode(double txm, uint8_t x, double tym, uint8_t y, double tzm, uint8_t z, double& t);
	void newNormal(uint8_t oldNode, uint8_t newNode, glm::dvec3& normal);
	bool readPoint(glm::dvec3 pos, glm::dvec3& color, int maxLevel);
	bool rayParam(Octree<glm::dvec3>* oct, const glm::dvec3& octPos, glm::dvec3 rayDir, glm::dvec3 pos, double lvl, Octree<glm::dvec3>** octreeHit, glm::dvec3& normal, 
		double& t, bool& hitOnEnter);
	bool procSubtree(glm::dvec3 t0, glm::dvec3 t1, Octree<glm::dvec3>* octree, const glm::dvec3& octPos,
		double lvl, uint8_t a, Octree<glm::dvec3>** octreeHit, glm::dvec3& normal, double& t, bool& hitOnEnter);

	inline void setLevels(uint8_t newLvl) { levels = newLvl; };

	bool generateData(int x, int y, Camera& camera, std::vector<std::vector<bool>>& inputs, double octSize, int radius, int& in, int& out);

	glm::dvec3 min, max;

private:
	VoxelMap worldMap;

	std::vector<glm::dvec3> pointLights;
	uint8_t levels;
	//7 -> 0.0078125
	//8 -> 0.00390625
	//9 -> 0.001953125
	double baseLvl = 1;
	//double topLevelSize = 1; Assuming this will always be 1 simplifies things a little

	glm::dvec3 lightColor = glm::dvec3(1.2, 1.2, 1.2);

	//glm::dvec3 gridOffset = glm::dvec3(10, 10, 10);
};