#include "VoxelScene.h"
#include <limits>
#include <algorithm>
#include <fstream>

VoxelScene::VoxelScene(int _levels)
{
	worldMap.map = new Octree<glm::dvec3>[MAP_WIDTH * MAP_HEIGHT * MAP_DEPTH];
	worldMap.width = MAP_WIDTH;
	worldMap.height = MAP_HEIGHT;
	worldMap.depth = MAP_DEPTH;

	levels = _levels;
}

VoxelScene::~VoxelScene()
{
	delete[] worldMap.map;
}

bool VoxelScene::rayParam(Octree<glm::dvec3>* oct, const glm::dvec3& octPos, glm::dvec3 rayDir, glm::dvec3 pos, double lvl, Octree<glm::dvec3>** octreeHit, glm::dvec3& normal, double& t, bool& hitOnEnter)
{
	uint8_t a = 0;

	double lvl2 = (double)lvl * 2.0;

	if (rayDir.x < 0.0f)
	{
		pos.x = (octPos.x + lvl2/2.0)*2 - pos.x;
		rayDir.x = -rayDir.x;
		a |= 4;
	}

	if (rayDir.y < 0.0f)
	{
		pos.y = (octPos.y + lvl2 / 2.0)*2 - pos.y;
		rayDir.y = -rayDir.y;
		a |= 2;
	}

	if (rayDir.z < 0.0f)
	{
		pos.z = (octPos.z + lvl2 / 2.0)*2 - pos.z;
		rayDir.z = -rayDir.z;
		a |= 1;
	}

	glm::dvec3 t0, t1;// IEEE stability fix

	//To avoid division by 0.
	for (uint8_t i = 0; i < 3; ++i)
	{
		if (rayDir[i] == 0.f)
		{
			rayDir[i] = std::numeric_limits<double>::epsilon();
		}
	}

	for (uint8_t i = 0; i < 3; ++i)
	{
		t0[i] = ((double)octPos[i] - pos[i]) / (double)rayDir[i];
		t1[i] = ((double)octPos[i] + lvl2 - pos[i]) / (double)rayDir[i];
	}

	if (std::max(std::max(t0[0], t0[1]), t0[2]) <= std::min(std::min(t1[0], t1[1]), t1[2]))
	{
		return procSubtree(t0, t1, oct, octPos, lvl, a, octreeHit, normal, t, hitOnEnter);
	}
	else {
		//std::cout << "error doubleing point precision " << std::max(std::max(t0[0], t0[1]), t0[2]) << " " << std::min(std::min(t1[0], t1[1]), t1[2]) << std::endl;
	}

	return false;
}

bool VoxelScene::procSubtree(glm::dvec3 t0, glm::dvec3 t1, Octree<glm::dvec3>* octree, const glm::dvec3& octPos, 
	double lvl, uint8_t a, Octree<glm::dvec3>** octreeHit, glm::dvec3& normal, double& t, bool& hitOnEnter)
{
	uint8_t currNode;

	if (t1[0] < 0.0f || t1[1] < 0.0f || t1[2] < 0.0f)
		return false;

	if (octree->contains == OCTREE_CONTENT::FILLED)
	{
		*octreeHit = octree;
		hitOnEnter = true;
		return true;
	}
	else if (octree->contains == OCTREE_CONTENT::EMPTY) 
	{
		return false;
	}

	glm::dvec3 tm;

	for (uint8_t i = 0; i < 3; ++i)
	{
		tm[i] = 0.5 * (t0[i] + t1[i]);
	}

	currNode = firstNode(t0, tm, t);

	glm::dvec3 tmp1;
	glm::dvec3 tmp2;

	uint8_t oldNode1 = currNode;
	uint8_t oldNode2 = currNode;
	bool hit = false;

	do {
		oldNode2 = oldNode1;
		oldNode1 = currNode;

		switch (currNode)
		{
		case 0:
			if (procSubtree(t0, tm, octree->tree + (int)(currNode ^ a), octPos, lvl, a, octreeHit, normal, t, hitOnEnter) == true)
			{
				hit = true;
			}
			else {
				currNode = newNode(tm.x, 4, tm.y, 2, tm.z, 1, t);
			}
			break;

		case 1:
			tmp1 = t0;
			tmp2 = tm;
			tmp1.z = tm.z;
			tmp2.z = t1.z;

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), octPos, lvl, a, octreeHit, normal, t, hitOnEnter) == true)
			{
				hit = true;
			}
			else {
				currNode = newNode(tm.x, 5, tm.y, 3, t1.z, 8, t);
			}
			break;

		case 2:
			tmp1 = t0;
			tmp2 = tm;
			tmp1.y = tm.y;
			tmp2.y = t1.y;

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), octPos, lvl, a, octreeHit, normal, t, hitOnEnter) == true)
			{
				hit = true;
			}
			else {
				currNode = newNode(tm.x, 6, t1.y, 8, tm.z, 3, t);
			}
			break;

		case 3:
			tmp1 = t0;
			tmp2 = tm;
			tmp1.y = tm.y;
			tmp2.y = t1.y;
			tmp1.z = tm.z;
			tmp2.z = t1.z;

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), octPos, lvl, a, octreeHit, normal, t, hitOnEnter) == true)
			{
				hit = true;
			}
			else {
				currNode = newNode(tm.x, 7, t1.y, 8, t1.z, 8, t);
			}
			break;

		case 4:
			tmp1 = t0;
			tmp2 = tm;
			tmp1.x = tm.x;
			tmp2.x = t1.x;

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), octPos, lvl, a, octreeHit, normal, t, hitOnEnter) == true)
			{
				hit = true;
			}
			else {
				currNode = newNode(t1.x, 8, tm.y, 6, tm.z, 5, t);
			}
			break;

		case 5:
			tmp1 = t0;
			tmp2 = tm;
			tmp1.x = tm.x;
			tmp2.x = t1.x;

			tmp1.z = tm.z;
			tmp2.z = t1.z;

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), octPos, lvl, a, octreeHit, normal, t, hitOnEnter) == true)
			{
				hit = true;
			}
			else {
				currNode = newNode(t1.x, 8, tm.y, 7, t1.z, 8, t);
			}
			break;

		case 6:
			tmp1 = t0;
			tmp2 = tm;
			tmp1.x = tm.x;
			tmp2.x = t1.x;
			tmp1.y = tm.y;
			tmp2.y = t1.y;

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), octPos, lvl, a, octreeHit, normal, t, hitOnEnter) == true)
			{
				hit = true;
			}
			else {
				currNode = newNode(t1.x, 8, t1.y, 8, tm.z, 7, t);
			}
			break;

		case 7:
			if (procSubtree(tm, t1, octree->tree + (int)(currNode ^ a), octPos, lvl, a, octreeHit, normal, t, hitOnEnter) == true)
			{
				hit = true;
			}
			else {
				currNode = 8;
			}
			
			break;
		}

		if (hitOnEnter == true && oldNode2 != currNode)
		{
			newNormal(oldNode2, currNode, normal);
			hitOnEnter = false;
		}

	} while (currNode < 8 && hit == false);



	return hit;
}

uint8_t VoxelScene::firstNode(glm::dvec3 t0, glm::dvec3 tm, double& t)
{
	uint8_t answer = 0;   // initialize to 00000000
	// select the entry plane and set bits
	if (t0.x > t0.y) {
		if (t0.x > t0.z) // PLANE YZ
		{ 
			if (tm.y < t0.x) answer |= 2;
			if (tm.z < t0.x) answer |= 1;

			t = t0.x;

			return answer;
		}
	}
	else {
		if (t0.y > t0.z) // PLANE XZ
		{ 
			if (tm.x < t0.y) answer |= 4;
			if (tm.z < t0.y) answer |= 1;

			t = t0.y;

			return answer;
		}
	}
	// PLANE XY
	if (tm.x < t0.z) answer |= 4;
	if (tm.y < t0.z) answer |= 2;

	t = t0.z;

	return answer;
}

uint8_t VoxelScene::newNode(double txm, uint8_t x, double tym, uint8_t y, double tzm, uint8_t z, double& t)
{
	if (txm < tym) 
	{
		if (txm < tzm) 
		{ 
			t = txm;
			return x; 
		}  // YZ plane
	}
	else {
		if (tym < tzm) 
		{ 
			t = tym;
			return y; 
		} // XZ plane
	}

	t = tzm;
	return z; // XY plane;
}

void VoxelScene::newNormal(uint8_t oldNode, uint8_t newNode, glm::dvec3& normal)
{
	if ((oldNode & 1) != (newNode & 1))
	{
		if ((oldNode & 1) == 1)
		{
			normal = glm::dvec3(0, 0, 1);
		}
		else {
			normal = glm::dvec3(0, 0, -1);
		}
	}
	else if ((oldNode & 2) != (newNode & 2)) 
	{
		if ((oldNode & 2) == 2)
		{
			normal = glm::dvec3(0, 1, 0);
		}
		else {
			normal = glm::dvec3(0, -1, 0);
		}
	}
	else if ((oldNode & 4) != (newNode & 4))
	{
		if ((oldNode & 4) == 4)
		{
			normal = glm::dvec3(1, 0, 0);
		}
		else {
			normal = glm::dvec3(-1, 0, 0);
		}
	}
}

//NOTE: when we get out of the bounds of the map by substracting 1, it will take the maximum value of an unsigned int
//which should be fine as long as the voxel map size is not equal to the max value of an unsigned int on any of the axis
//NOTE2: hitPos needs to be fixed
bool VoxelScene::traceRay(VoxelMap& map, const glm::dvec3& rayDir, const glm::dvec3& pos, Octree<glm::dvec3>** octreeHit, glm::dvec3& hitPos,
	glm::dvec3& normal)
{
	glm::uvec3 mapPos;//Case in which the ray currently is
	glm::dvec3 gridPos;//Worldpos of the voxel, used to know how we have traveled (in euclidean distance)

	glm::uvec3 test;

	Axis side = Axis::X;//Axis on which the the voxel was hit

	//When entering the grid

	//Currently assumes we are in the grid at the start
	mapPos.x = int(pos.x);
	mapPos.y = int(pos.y);
	mapPos.z = int(pos.z);

	gridPos = mapPos;

	//In which direction we're going for each axis (either +1, or -1)
	glm::ivec3 step;

	glm::dvec3 sideDist;
	glm::dvec3 deltaDist;

	//to compress in a loop
	//Distance to another case on each axis
	deltaDist.x = fabs(1 / rayDir.x);
	deltaDist.y = fabs(1 / rayDir.y);
	deltaDist.z = fabs(1 / rayDir.z);

	//to compress in a loop
	//calculate step and initial sideDist
	if (rayDir.x < 0)
	{
		step.x = -1;
		sideDist.x = (pos.x - gridPos.x) * deltaDist.x;
	}
	else
	{
		step.x = 1;
		sideDist.x = (gridPos.x + 1.0f - pos.x) * deltaDist.x;
	}

	if (rayDir.y < 0)
	{
		step.y = -1;
		sideDist.y = (pos.y - gridPos.y) * deltaDist.y;
	}
	else
	{
		step.y = 1;
		sideDist.y = (gridPos.y + 1.0f - pos.y) * deltaDist.y;
	}

	if (rayDir.z < 0)
	{
		step.z = -1;
		sideDist.z = (pos.z - gridPos.z) * deltaDist.z;
	}
	else
	{
		step.z = 1;
		sideDist.z = (gridPos.z + 1.0f - pos.z) * deltaDist.z;
	}

	bool hit = false; //was there a wall hit?

	glm::dvec3 prevPos = gridPos;

	//Test case by case
	bool move = false;//If we passed through a sparse octree without hitting anything 
	while (hit == false && mapPos.x < map.width && mapPos.x >= 0 && mapPos.y < map.height && mapPos.y >= 0 && mapPos.z < map.depth && mapPos.z >= 0)
	{
		Octree<glm::dvec3>* currentTree = (map.map + mapPos.x * map.height * map.depth + mapPos.y * map.depth + mapPos.z);

		//Check if ray has hit a wall
		if(currentTree->contains == OCTREE_CONTENT::EMPTY || move == true)//Empty or not hit on sparse octree
		{
			move = false;

			prevPos = gridPos;

			//jump to next map square, OR in x-direction, OR in y-direction
			if (sideDist.x < sideDist.y)
			{
				if (sideDist.x < sideDist.z)
				{
					sideDist.x += deltaDist.x;
					mapPos.x += step.x;
					gridPos.x += step.x * baseLvl;
					side = Axis::X;
				}
				else {
					sideDist.z += deltaDist.z;
					mapPos.z += step.z;
					gridPos.z += step.z * baseLvl;
					side = Axis::Z;
				}
			}
			else
			{
				if (sideDist.y < sideDist.z)
				{
					sideDist.y += deltaDist.y;
					mapPos.y += step.y;
					gridPos.y += step.y * baseLvl;
					side = Axis::Y;
				}
				else {
					sideDist.z += deltaDist.z;
					mapPos.z += step.z;
					gridPos.z += step.z * baseLvl;
					side = Axis::Z;
				}
			}
		}
		else if(currentTree->contains == OCTREE_CONTENT::FILLED)
		{
			hit = true;
			*octreeHit = currentTree;

			double halfLvl = baseLvl / 2;
			double t;

			for (uint8_t i = 0; i < 3; ++i)
			{
				if (i != (uint8_t)side)
				{
					normal[i] = 0;
				}
				else {
					normal[i] = -step[i];

					if (rayDir[i] >= 0)
					{
						double rayDirection = rayDir[i];

						if (rayDir[i] == 0) rayDirection += std::numeric_limits<double>::epsilon();

						t = (gridPos[i] - pos[i]) / rayDirection;
					}
					else {
						t = (gridPos[i] + baseLvl * 2.f - pos[i]) / rayDir[i];
					}
				}
			}
			
			hitPos = rayDir * (t - std::numeric_limits<float>::epsilon()) + pos;//Get intersect pos
		}
		else//Sparse octree
		{

			double t;
			bool hitOnEnter = false;

			hit = rayParam(currentTree, gridPos, rayDir, pos, baseLvl / 2.0, octreeHit, normal, t, hitOnEnter);

			if (hit == true)
			{
				hitPos = rayDir * (t - std::numeric_limits<float>::epsilon()) + pos;

				if (hitOnEnter == true)
				{
					for (uint8_t i = 0; i < 3; ++i)
					{
						if (i != (uint8_t)side)
						{
							normal[i] = 0;
						}
						else {
							normal[i] = -step[i];
						}
					}
				}
			}

			move = !hit;//if false move like if it was empty
		}
	}

	return hit;
}

void VoxelScene::drawPixels(int workload, int x, int y, Window& window, Camera& camera, std::vector<std::vector<glm::vec3>>& buffer, 
	double octSize, int radius, Hyperneat* hyperneat, Genome* gen, std::atomic<bool>* ticket)
{
	for (int i = 0; i < workload; i++)
	{
		//Compute position and direction of a ray
		double xx = (2.f * ((x + 0.5f) * camera.wdithStep) - 1.f) * camera.angle * camera.aspectratio;
		double yy = (1.f - 2.f * ((y + 0.5f) * camera.heightStep)) * camera.angle;

		glm::dvec3 rayDir = glm::normalize(camera.camRot * glm::vec3(xx, yy, 1.f));

		glm::dvec3 color;
		Octree<glm::dvec3>* octreeHit = nullptr;
		glm::dvec3 hitPos;
		glm::dvec3 normal;

		if (traceRay(worldMap, rayDir, camera.pos, &octreeHit, hitPos, normal) == true)
		{
			color = *octreeHit->object;

			if (hyperneat != nullptr)
			{
				std::vector<float> inputs;
				std::vector<float> outputs;
				glm::dvec3 posRef(floor(hitPos.x / octSize) * octSize, floor(hitPos.y / octSize) * octSize, floor(hitPos.z / octSize) * octSize);
				glm::dvec3 colorHolder;
				glm::dvec3 readPos;
				glm::dvec3 inputNetwork;
				glm::dvec3 pointPos;

				std::vector<std::vector<float>> inputsPos;

				double maxDistPlus = (radius + 1) * octSize;
				double maxDist = radius * octSize;

				for (double x = -radius; x <= radius; x++)
				{
					readPos.x = hitPos.x + x * octSize;
					inputNetwork.x = (1 - abs(x * octSize) / maxDistPlus) * (x < 0 ? 1 : -1);
					pointPos.x = x * octSize;

					for (double y = -radius; y <= radius; y++)
					{
						readPos.y = hitPos.y + y * octSize;
						inputNetwork.y = (1 - abs(y * octSize) / maxDistPlus) * (y < 0 ? 1 : -1);
						pointPos.y = y * octSize;

						for (double z = -radius; z <= radius; z++)
						{
							readPos.z = hitPos.z + z * octSize;
							inputNetwork.z = (1 - abs(z * octSize) / maxDistPlus) * (z < 0 ? 1 : -1);
							pointPos.z = z * octSize;

							if (maxDist >= glm::length(pointPos))
							{
								inputsPos.push_back(std::vector<float>());

								for (int axis = 0; axis < 3; axis++)
								{
									
									inputsPos.back().push_back(inputNetwork[axis]);
								}

								inputs.push_back((readPoint(readPos, colorHolder, levels) == true ? 1 : 0));
							}
						}
					}
				}

				std::vector<std::vector<std::vector<float>>> hiddenSubstrate;
				std::vector<std::vector<float>> outputSubstrate;
				outputSubstrate.push_back(std::vector<float>());
				outputSubstrate.push_back(std::vector<float>());
				outputSubstrate.push_back(std::vector<float>());
				outputSubstrate[0].push_back(1);
				outputSubstrate[0].push_back(0);
				outputSubstrate[0].push_back(0);

				outputSubstrate[1].push_back(0);
				outputSubstrate[1].push_back(1);
				outputSubstrate[1].push_back(0);

				outputSubstrate[2].push_back(0);
				outputSubstrate[2].push_back(0);
				outputSubstrate[2].push_back(1);

				NeuralNetwork network;
				hyperneat->genomeToNetwork(*gen, network, inputsPos, outputSubstrate, hiddenSubstrate);
				network.compute(inputs, outputs);

				for (int axis = 0; axis < 3; axis++)
				{
					normal[axis] = outputs[axis];
				}

				normal = glm::normalize(normal);
			}

			bool hitByLight = true;/////////////////////////////////////////////////

			//double mDist = -glm::dot(-normal, hitPos);

			//for (int i = 0; i < pointLights.size() && hitByLight == false; ++i)
			//{
			//	//Check if the on which side of the plane the light is
			//	if ((glm::dot(-normal, pointLights[i]) + mDist) < 0)
			//	{
			//		glm::dvec3 filler, filler2;
			//		Octree<glm::dvec3>* oHitLight = nullptr;

			//		rayDir = glm::normalize(hitPos - glm::dvec3(pointLights[i]));

			//		traceRay(worldMap, rayDir, pointLights[i], &oHitLight, filler, filler2);

			//		if (octreeHit == oHitLight)
			//		{
			//			hitByLight = true;

			//			double ambientStrength = 0.00f;
			//			glm::dvec3 ambient = lightColor * ambientStrength;
			//			// diffuse 
			//			//normal = -normal;
			//			glm::dvec3 lightDir = glm::normalize(pointLights[i] - hitPos);
			//			double diff = std::max(glm::dot(normal, lightDir), 0.f);
			//			glm::dvec3 diffuse = lightColor * diff;
			//			// specular
			//			double specularStrength = 0.15;
			//			glm::dvec3 viewDir = glm::normalize(camera.pos - hitPos);
			//			glm::dvec3 reflectDir = reflect(-lightDir, normal);
			//			double spec = pow(std::max(dot(viewDir, reflectDir), 0.f), 32.f);
			//			glm::dvec3 specular = lightColor * specularStrength * spec;
			//			glm::dvec3 result = (ambient + diffuse) * color + specular;

			//			buffer[x][y] = glm::min(result, glm::dvec3(1.f));
			//			//buffer[x][y] = abs(normal);
			//			//buffer[x][y] = color;
			//		}
			//	}
			//}

			buffer[x][y] = abs(normal);

			if (hitByLight == false)
			{
				buffer[x][y] = glm::dvec3(0, 0, 0);
			}
			
		}
		else {
			buffer[x][y] = glm::vec3(0.05, 0.05, 0.05);
		}

		y++;

		if (y >= camera.windowHeight)
		{
			x++;
			y -= camera.windowHeight;
		}
	}

	if (ticket != nullptr)
	{
		(*ticket) = true;
		ticket->notify_one();
	}
}

bool VoxelScene::generateData(int x, int y, Camera& camera,
	std::vector<std::vector<bool>>& inputs, double octSize, int radius, int& in, int& out)
{
	//Compute position and direction of a ray
	double xx = (2.f * ((x + 0.5f) * camera.wdithStep) - 1.f) * camera.angle * camera.aspectratio;
	double yy = (1.f - 2.f * ((y + 0.5f) * camera.heightStep)) * camera.angle;

	glm::dvec3 rayDir = glm::normalize(camera.camRot * glm::vec3(xx, yy, 1.f));

	glm::dvec3 color;
	Octree<glm::dvec3>* octreeHit = nullptr;
	glm::dvec3 hitPos;
	glm::dvec3 normal;

	if (traceRay(worldMap, rayDir, camera.pos, &octreeHit, hitPos, normal) == true)
	{
		inputs.push_back(std::vector<bool>());

		glm::dvec3 posRef(floor(hitPos.x / octSize) * octSize, floor(hitPos.y / octSize) * octSize, floor(hitPos.z / octSize) * octSize);
		glm::dvec3 colorHolder;
		glm::dvec3 readPos;
		glm::dvec3 inputNetwork;
		glm::dvec3 pointPos;

		double maxDistPlus = (radius + 1) * octSize;
		double maxDist = radius * octSize;

		glm::vec3 test(0);

		for (double x = -radius; x <= radius; x++)
		{
			readPos.x = hitPos.x + x * octSize;
			inputNetwork.x = (1 - abs(x * octSize) / maxDistPlus) * (x < 0 ? 1 : -1);
			pointPos.x = x * octSize;

			for (double y = -radius; y <= radius; y++)
			{
				readPos.y = hitPos.y + y * octSize;
				inputNetwork.y = (1 - abs(y * octSize) / maxDistPlus) * (y < 0 ? 1 : -1);
				pointPos.y = y * octSize;

				for (double z = -radius; z <= radius; z++)
				{
					readPos.z = hitPos.z + z * octSize;
					inputNetwork.z = (1 - abs(z * octSize) / maxDistPlus) * (z < 0 ? 1 : -1);
					pointPos.z = z * octSize;

					if (maxDist >= glm::length(pointPos))
					{
						inputs.back().push_back(readPoint(readPos, colorHolder, levels));

						if (inputs.back().back() == true)
						{
							test += inputNetwork;
						}

						if (z == 0 && x == 0 && y == 0)
						{
							if (inputs.back().back() == true)
							{
								in++;
							}
							else {
								out++;
							}
						}
					}
				}
			}
		}

		//std::cout << glm::normalize(test) << std::endl;

		return true;
	}

	return false;
}

bool VoxelScene::addPoint(glm::dvec3 pos, glm::dvec3 color)
{
	uint8_t level = 1;
	double divLevel = 0.5f;

	if (pos.x < 0 || pos.x >= worldMap.width || pos.y < 0 || pos.y >= worldMap.height || pos.z < 0 || pos.z >= worldMap.depth) return false;

	Octree<glm::dvec3> *currentTree = (worldMap.map + int(pos.x) * worldMap.height * worldMap.depth + int(pos.y) * worldMap.depth + int(pos.z));

	//Check that we are within bounds

	for (uint8_t i = 0; i < 3; i++)
	{
		pos[i] -= int(pos[i]);
	}

	while (level < levels && currentTree->contains != OCTREE_CONTENT::FILLED)
	{
		//If empty create tree
		if (currentTree->contains == OCTREE_CONTENT::EMPTY)
		{
			currentTree->tree = new Octree<glm::dvec3>[8];
			currentTree->contains = OCTREE_CONTENT::SPARSE;
		}

		level++;

		uint8_t index = 0;
		
		for (uint8_t i = 0; i < 3; i++)
		{
			if (pos[i] > divLevel)
			{
				pos[i] -= divLevel;

				index |= 1 << (2 - i);
			}
		}

		currentTree = currentTree->tree + index;
		divLevel *= 0.5f;
	}

	//If empty create tree
	if (currentTree->contains == OCTREE_CONTENT::EMPTY)
	{
		currentTree->tree = new Octree<glm::dvec3>[8];
		currentTree->contains = OCTREE_CONTENT::SPARSE;
	}

	if (currentTree->contains != OCTREE_CONTENT::FILLED)//Should be empty or filled if we are at max level 
	{
		currentTree->contains = OCTREE_CONTENT::FILLED;
		currentTree->object = new glm::dvec3(color);

		return true;
	}

	//std::cout << "already filled\n";
	return false;
}

bool VoxelScene::readPoint(glm::dvec3 pos, glm::dvec3& color, int maxLevel)
{
	uint8_t level = 1;
	double divLevel = 0.5f;

	if (pos.x < 0 || pos.x >= worldMap.width || pos.y < 0 || pos.y >= worldMap.height || pos.z < 0 || pos.z >= worldMap.depth) return false;

	Octree<glm::dvec3>* currentTree = (worldMap.map + int(pos.x) * worldMap.height * worldMap.depth + int(pos.y) * worldMap.depth + int(pos.z));

	//Check that we are within bounds

	for (uint8_t i = 0; i < 3; i++)
	{
		pos[i] -= int(pos[i]);
	}

	while (level < maxLevel && currentTree->contains != OCTREE_CONTENT::FILLED && currentTree->contains != OCTREE_CONTENT::EMPTY)
	{
		level++;

		uint8_t index = 0;

		for (uint8_t i = 0; i < 3; i++)
		{
			if (pos[i] > divLevel)
			{
				pos[i] -= divLevel;

				index |= 1 << (2 - i);
			}
		}

		currentTree = currentTree->tree + index;
		divLevel *= 0.5f;
	}

	//If empty create tree
	if (currentTree->contains == OCTREE_CONTENT::EMPTY)
	{
		return false;
	}

	color = *currentTree->object;
	return true;
}

void VoxelScene::simplify()
{
	for (int x = 0; x < worldMap.width; x++)
	{
		for (int y = 0; y < worldMap.height; y++)
		{
			for (int z = 0; z < worldMap.depth; z++)
			{
				Octree<glm::dvec3>* currentTree = (worldMap.map + x * worldMap.height * worldMap.depth + y * worldMap.depth + z);

				if (currentTree->contains == OCTREE_CONTENT::SPARSE)
				{
					simplifyOctree(currentTree);
				}
			}
		}
	}
}

void VoxelScene::simplifyOctree(Octree<glm::dvec3>* tree)
{
	glm::dvec3 *color = nullptr;

	//Check if leaves are similar
	for (uint8_t i = 0; i < 8; i++)
	{
		if (tree->contains == OCTREE_CONTENT::SPARSE)
		{
			simplifyOctree(tree->tree);

			if (tree->contains == OCTREE_CONTENT::SPARSE)
			{
				return;
			}
		}

		if (tree->contains == OCTREE_CONTENT::EMPTY)
		{
			return;
		}
		else {
			if (i == 0)
			{
				color = tree->object;
			}
			else if (*color != *tree->object)
			{
				return;
			}
		}
	}

	//Delete tree
	delete[] tree->tree;

	tree->contains = OCTREE_CONTENT::FILLED;
	tree->object = new glm::dvec3(*color);

	//std::cout << "merged\n";
}

bool VoxelScene::loadModel(glm::dvec3 pos, std::string file)
{
	std::fstream fs(file, std::fstream::in);

	if (fs.is_open() == true)
	{
		std::cout << "Loading model\n";

		char buff[256];
		std::string str = "";
		char tmp;

		std::string s1, s2, s3;
		std::string s4, s5, s6;

		bool init = true;

		while (fs.eof() == false)
		{
			fs >> s1 >> s2 >> s3 >> s4 >> s5 >> s6;

			//I think there's a small bug with the loading of the model, probably due to doubleing point imprecision
			//Adding a very small offset makes it disappear
			glm::dvec3 point = glm::dvec3(stod(s1), stod(s2), stod(s3)) + pos;

			addPoint(glm::dvec3(stod(s1), stod(s2), stod(s3)) + pos, glm::dvec3(stof(s4), stof(s5), stof(s6)));

			for (uint8_t i = 0; i < 3; i++)
			{
				if (min[i] > point[i] || init == true)
				{
					min[i] = point[i];
				}

				if (max[i] < point[i] || init == true)
				{
					max[i] = point[i];
				}
			}

			init = false;
		}

		fs.close();

		for (uint8_t i = 0; i < 3; i++)
		{
			std::cout << min[i] << " ";
		}

		std::cout << std::endl;

		for (uint8_t i = 0; i < 3; i++)
		{
			std::cout << max[i] << " ";
		}

		std::cout << std::endl;

		return true;
	}

	return false;
}