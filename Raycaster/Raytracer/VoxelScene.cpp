#include "VoxelScene.h"
#include <limits>
#include <algorithm>

VoxelScene::VoxelScene()
{
	worldMap.map = new Octree<glm::vec3>[MAP_WIDTH * MAP_HEIGHT * MAP_DEPTH];
	worldMap.width = MAP_WIDTH;
	worldMap.height = MAP_HEIGHT;
	worldMap.depth = MAP_DEPTH;

	//(worldMap.map + 1 * worldMap.height * worldMap.depth + 1 * worldMap.depth + 3)->object = new glm::vec3(0, 0.3, 0);
	//(worldMap.map + 1 * worldMap.height * worldMap.depth + 1 * worldMap.depth + 3)->contains = OCTREE_CONTENT::FILLED;

	//(worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->object = new glm::vec3(0.5, 0, 0);
	//(worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->contains = OCTREE_CONTENT::FILLED;

	(worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree = new Octree<glm::vec3>[8];
	(worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->contains = OCTREE_CONTENT::SPARSE;
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 0 * 4 + 0 * 2 + 0)->object = new glm::vec3(0.5, 0, 0);
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 0 * 4 + 0 * 2 + 0)->contains = OCTREE_CONTENT::FILLED;

	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 0 * 2 * 2 + 0 * 2 + 0)->object = new glm::vec3(0.5, 0, 0);
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 0 * 2 * 2 + 0 * 2 + 0)->contains = OCTREE_CONTENT::FILLED;
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 0)->object = new glm::vec3(0, 0.5, 0);
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 0)->contains = OCTREE_CONTENT::FILLED;
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 1)->object = new glm::vec3(0.5, 0, 0);
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 1)->contains = OCTREE_CONTENT::FILLED;
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 2)->object = new glm::vec3(0, 0, 0.5);
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 2)->contains = OCTREE_CONTENT::FILLED;
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 3)->object = new glm::vec3(0.5, 0.5, 0.5);
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 3)->contains = OCTREE_CONTENT::FILLED;
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 4)->object = new glm::vec3(0.5, 0.5, 0);
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 4)->contains = OCTREE_CONTENT::FILLED;
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 5)->object = new glm::vec3(0, 0.5, 0.5);
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 5)->contains = OCTREE_CONTENT::FILLED;
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 6)->object = new glm::vec3(0.5, 0, 0.5);
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 6)->contains = OCTREE_CONTENT::FILLED;
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 7)->object = new glm::vec3(1, 1, 1);
	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 2)->tree + 7)->contains = OCTREE_CONTENT::FILLED;

	//((worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 0)->tree = new Octree<glm::vec3>[8];
	//((worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 0)->contains = OCTREE_CONTENT::SPARSE;
	//(((worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 0)->tree + 0)->object = new glm::vec3(0.3, 0.3, 0.3);
	//(((worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 0)->tree + 0)->contains = OCTREE_CONTENT::FILLED;



	//for(int i = 0; i < 8; ++i)
	//{
	//	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + i)->object = new Color(128, 0, 0);
	//	((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + i)->contains = OCTREE_CONTENT::FILLED;
	//}

	//(worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->object->r = 128;

	//std::cout << *(worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->object << std::endl;

	for (int x = 20; x < 24; x++)
	{
		for (int y = 1; y < 3; y++)
		{
			for (int z = 20; z < 24; z++)
			{
				//worldMap[x][y][z].isEmpty = false;
				//worldMap[x][y][z].color.r = 128;
			}
		}
	}
}

VoxelScene::~VoxelScene()
{
	delete[] worldMap.map;
}

bool VoxelScene::rayParam(Octree<glm::vec3>* oct, const glm::vec3& octPos, glm::vec3 rayDir, glm::vec3 pos, double lvl, glm::vec3* color, glm::vec3& normal)
{
	//std::cout << octPos << std::endl;

	uint8_t a = 0;

	float lvl2 = lvl * 2;
	glm::vec3 min, max;

	for (uint8_t i = 0; i < 3; ++i)
	{
		//min[i] = -lvl;
		//max[i] = lvl;
		min[i] = octPos[i] - lvl;
		max[i] = octPos[i] + lvl2;
	}

	if (rayDir.x < 0.0f)
	{
		//pos.x = (octPos.x + lvl) * 2 - pos.x;
		//pos.x = octPos.x * 2 - pos.x;
		pos.x = lvl2 - pos.x;
		rayDir.x = -rayDir.x;
		a |= 1;
	}

	if (rayDir.y < 0.0f)
	{
		//pos.y = (octPos.y + lvl) * 2 - pos.y;
		//pos.y = octPos.y * 2 - pos.y;
		pos.y = lvl2 - pos.y;
		rayDir.y = -rayDir.y;
		a |= 2;
	}

	if (rayDir.z < 0.0f)
	{
		//pos.z = (octPos.z + lvl) * 2 - pos.z;
		//pos.z = octPos.z * 2 - pos.z;
		pos.z = lvl2 - pos.z;
		rayDir.z = -rayDir.z;
		a |= 4;
	}

	glm::dvec3 t0, t1;// IEEE stability fix

	for (uint8_t i = 0; i < 3; ++i)
	{
		t0[i] = ((double)octPos[i] - pos[i]) / (double)rayDir[i];
		t1[i] = ((double)octPos[i] + lvl*2.0 - pos[i]) / (double)rayDir[i];

		//if (i == 2)
		{
			//std::cout << min[i] << " " << max[i] << " " << pos[i] << " " << rayDir[i] << std::endl;

		}
		//
	}

	if (std::max(std::max(t0[0], t0[1]), t0[2]) < std::min(std::min(t1[0], t1[1]), t1[2]))
	{
		return procSubtree(t0, t1, oct, a, color, normal);
	}
	else {
		std::cout << "error\n";
	}

	return false;
}

bool VoxelScene::procSubtree(glm::dvec3 t0, glm::dvec3 t1, Octree<glm::vec3>* octree, uint8_t a, glm::vec3* color, glm::vec3& normal)
{
	uint8_t currNode;

	if (t1[0] < 0.0f || t1[1] < 0.0f || t1[2] < 0.0f)
		return false;

	if (octree->contains == OCTREE_CONTENT::FILLED)
	{
		//proc_terminal(n);
		*color = *octree->object;
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

	currNode = firstNode(t0, tm);

	//std::cout << t0 << " " << tm << " " << (int)currNode << std::endl;

	glm::vec3 tmp1;
	glm::vec3 tmp2;

	uint8_t oldNode;

	do {
		oldNode = currNode;
		
		switch (currNode)
		{
		case 0:
			if(procSubtree(t0, tm, octree->tree + (int)(currNode ^ a), a, color, normal) == true) return true;

			currNode = newNode(tm.x, 4, tm.y, 2, tm.z, 1);
			break;

		case 1:
			tmp1 = t0;
			tmp2 = tm;
			tmp1[2] = tm[2];
			tmp2[2] = t1[2];

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), a, color, normal) == true) return true;

			currNode = newNode(tm.x, 5, tm.y, 3, t1.z, 8);
			break;

		case 2:
			tmp1 = t0;
			tmp2 = tm;
			tmp1[1] = tm[1];
			tmp2[1] = t1[1];

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), a, color, normal) == true) return true;

			currNode = newNode(tm.x, 6, t1.y, 8, tm.z, 3);
			break;

		case 3:
			tmp1 = t0;
			tmp2 = tm;
			tmp1[1] = tm[1];
			tmp2[1] = t1[1];
			tmp1[2] = tm[2];
			tmp2[2] = t1[2];

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), a, color, normal) == true) return true;

			currNode = newNode(tm.x, 7, t1.y, 8, t1.z, 8);
			break;

		case 4:
			tmp1 = t0;
			tmp2 = tm;
			tmp1[0] = tm[0];
			tmp2[0] = t1[0];

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), a, color, normal) == true) return true;

			currNode = newNode(t1.x, 8, tm.y, 6, tm.z, 5);
			break;

		case 5:
			tmp1 = t0;
			tmp2 = tm;
			tmp1[0] = tm[0];
			tmp2[0] = t1[0];

			tmp1[2] = tm[2];
			tmp2[2] = t1[2];

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), a, color, normal) == true) return true;

			currNode = newNode(t1.x, 8, tm.y, 7, t1.z, 8);
			break;

		case 6:
			tmp1 = t0;
			tmp2 = tm;
			tmp1[0] = tm[0];
			tmp2[0] = t1[0];
			tmp1[1] = tm[1];
			tmp2[1] = t1[1];

			if(procSubtree(tmp1, tmp2, octree->tree + (int)(currNode ^ a), a, color, normal) == true) return true;

			currNode = newNode(t1.x, 8, t1.y, 8, tm.z, 7);
			break;

		case 7:
			if (procSubtree(tm, t1, octree->tree + (int)(currNode ^ a), a, color, normal) == true) return true;
			currNode = 8;
			break;
		}

		//std::cout << (int)oldNode << " " << (int)currNode << std::endl;

		//newNormal(oldNode, currNode, normal);

	} while (currNode < 8);

	return false;
}

uint8_t VoxelScene::firstNode(glm::dvec3 t0, glm::dvec3 tm)
{
	//std::cout << t0 << " " << tm << " " << (int)((tm[0] < t0[2] ? 1 : 0) | (tm[1] < t0[2] ? 2 : 0)
	//	| (tm[0] < t0[1] ? 1 : 0) | (tm[2] < t0[1] ? 4 : 0)
	//	| (tm[1] < t0[0] ? 2 : 0) | (tm[2] < t0[0] ? 4 : 0)) << std::endl;

	//return ((tm[0] < t0[2] ? 1 : 0) | (tm[1] < t0[2] ? 2 : 0) 
	//	| (tm[0] < t0[1] ? 1 : 0) | (tm[2] < t0[1] ? 4 : 0) 
	//	| (tm[1] < t0[0] ? 2 : 0) | (tm[2] < t0[0] ? 4 : 0));

	uint8_t answer = 0;   // initialize to 00000000
	// select the entry plane and set bits
	if (t0.x > t0.y) {
		if (t0.x > t0.z) { // PLANE YZ
			if (tm.z < t0.x) answer |= 4;
			if (tm.y < t0.x) answer |= 2;
			return answer;
		}
	}
	else {
		if (t0.y > t0.z) { // PLANE XZ
			if (tm.z < t0.y) answer |= 4;    // set bit at position 2
			if (tm.x < t0.y) answer |= 1;    // set bit at position 0
			return answer;
		}
	}
	// PLANE XY
	if (tm.y < t0.z) answer |= 2;
	if (tm.x < t0.z) answer |= 1;
	return answer;
}

uint8_t VoxelScene::newNode(float txm, uint8_t x, float tym, uint8_t y, float tzm, uint8_t z)
{
	if (txm < tym) {
		if (txm < tzm) { return x; }  // YZ plane
	}
	else {
		if (tym < tzm) { return y; } // XZ plane
	}
	return z; // XY plane;
}

void VoxelScene::newNormal(uint8_t oldNode, uint8_t newNode, glm::vec3& normal)
{
	if ((oldNode & 1) != (newNode & 1))
	{
		if ((oldNode & 1) == 1)
		{
			normal = glm::vec3(0, 0, 1);
		}
		else {
			normal = glm::vec3(0, 0, -1);
		}
	}
	else if ((oldNode & 2) != (newNode & 2)) 
	{
		if ((oldNode & 2) == 2)
		{
			normal = glm::vec3(0, 1, 0);
		}
		else {
			normal = glm::vec3(0, -1, 0);
		}
	}
	else if ((oldNode & 4) != (newNode & 4))
	{
		if ((oldNode & 4) == 4)
		{
			normal = glm::vec3(1, 0, 0);
		}
		else {
			normal = glm::vec3(-1, 0, 0);
		}
	}
}

//NOTE: when we get out of the bounds of the map by substracting 1, it will take the maximum value of an unsigned int
//which should be fine as long as the voxel map size is not equal to the max value of an unsigned int on any of the axis
//NOTE2: hitPos needs to be fixed
bool VoxelScene::traceRay(VoxelMap& map, const glm::vec3& rayDir, const glm::vec3& pos, glm::vec3& color, glm::vec3& hitPos,
	glm::vec3& normal, glm::vec3* destination, double lvl, MapEntrance* entrance)
{
	glm::uvec3 mapPos;//Case in which the ray currently is
	glm::vec3 gridPos;//Worldpos of the voxel, used to know how we have traveled (in euclidean distance)
	double sqrLvl = lvl/2 * lvl/2;

	glm::uvec3 test;

	Axis side = Axis::X;//Axis on which the the voxel was hit

	//When entering the grid
	if (entrance == nullptr)
	{
		//Currently assumes we are in the grid at the start
		mapPos.x = int(pos.x);
		mapPos.y = int(pos.y);
		mapPos.z = int(pos.z);

		gridPos = mapPos;
	}

	//In which direction we're going for each axis (either +1, or -1)
	glm::ivec3 step;

	glm::vec3 sideDist;
	glm::vec3 deltaDist;

	//to compress in a loop
	//Distance to another case on each axis
	deltaDist.x = fabs(1 / rayDir.x);
	deltaDist.y = fabs(1 / rayDir.y);
	deltaDist.z = fabs(1 / rayDir.z);

	//deltaDist /= lvl;

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

	glm::vec3 prevPos = gridPos;

	//Test case by case
	bool move = false;//If we passed through a sparse octree without hitting anything 
	while (hit == false && mapPos.x < map.width && mapPos.x >= 0 && mapPos.y < map.height && mapPos.y >= 0 && mapPos.z < map.depth && mapPos.z >= 0)
	{
		Octree<glm::vec3>* currentTree = (map.map + mapPos.x * map.height * map.depth + mapPos.y * map.depth + mapPos.z);

		//Check if ray has hit a wall
		if(currentTree->contains == OCTREE_CONTENT::EMPTY || move == true)//Empty or not hit on sparse octree
		{
			move = false;

			//if (destination != nullptr && glm::distance2(gridPos, *destination) < sqrLvl)
			//{
			//	//std::cout << gridPos << " " << destination << std::endl;
			//	break;
			//}

			prevPos = gridPos;

			//jump to next map square, OR in x-direction, OR in y-direction
			if (sideDist.x < sideDist.y)
			{
				if (sideDist.x < sideDist.z)
				{
					sideDist.x += deltaDist.x;
					mapPos.x += step.x;
					gridPos.x += step.x * lvl;
					side = Axis::X;
				}
				else {
					sideDist.z += deltaDist.z;
					mapPos.z += step.z;
					gridPos.z += step.z * lvl;
					side = Axis::Z;
				}
			}
			else
			{
				if (sideDist.y < sideDist.z)
				{
					sideDist.y += deltaDist.y;
					mapPos.y += step.y;
					gridPos.y += step.y * lvl;
					side = Axis::Y;
				}
				else {
					sideDist.z += deltaDist.z;
					mapPos.z += step.z;
					gridPos.z += step.z * lvl;
					side = Axis::Z;
				}
			}
		}
		else if(currentTree->contains == OCTREE_CONTENT::FILLED)
		{

			hit = true;
			color = *currentTree->object;

			float halfLvl = lvl / 2;
			float t;

			for (uint8_t i = 0; i < 3; ++i)
			{
				if (i != (uint8_t)side)
				{
					normal[i] = 0;
				}
				else {
					gridPos[i] -= halfLvl * (step[i] + std::numeric_limits<float>::epsilon() * 100);
					normal[i] = -step[i];
				}
			}

			intersectPlane(normal, gridPos, pos, rayDir, t);
			
			//std::cout << mapPos << std::endl;

			//std::cout << lvl << std::endl;
			
			hitPos = rayDir * (t - std::numeric_limits<float>::epsilon()*100) + pos;//Get intersect pos relative to the octree
		}
		else//Sparse octree
		{
			//float newLvl = lvl / 2;
			//MapEntrance newEntrance;

			//newEntrance.pos = gridPos;

			//for (uint8_t i = 0; i < 3; ++i)
			//{
			//	if (i != (uint8_t)side)
			//	{
			//		newEntrance.normal[i] = 0;
			//	}
			//	else {
			//		newEntrance.normal[i] = -step[i];
			//		//newEntrance.pos[i] -= newLvl * step[i];
			//		//float tmp = newEntrance.pos[i] - newEntrance.pos[i] / lvl;
			//		//newEntrance.pos[i] = newEntrance.pos[i] - tmp;
			//		//newEntrance.pos[i] += tmp < newLvl ? newLvl : -newLvl;
			//	}
			//}

			//VoxelMap map;

			//map.map = currentTree->tree;
			//map.depth = 2;
			//map.height = 2;
			//map.width = 2;

			//hit = traceRay(map, rayDir, pos, color, hitPos, normal, destination, newLvl, &newEntrance);

			for (uint8_t i = 0; i < 3; ++i)
			{
				if (i != (uint8_t)side)
				{
					normal[i] = 0;
				}
				else {
					normal[i] = -step[i];
					//newEntrance.pos[i] -= newLvl * step[i];
					//float tmp = newEntrance.pos[i] - newEntrance.pos[i] / lvl;
					//newEntrance.pos[i] = newEntrance.pos[i] - tmp;
					//newEntrance.pos[i] += tmp < newLvl ? newLvl : -newLvl;
				}
			}

			//std::cout << normal << std::endl;

			hit = rayParam(currentTree, gridPos, rayDir, pos, lvl / 2.0, &color, normal);

			//std::cout << normal << std::endl;

			move = !hit;//if false move like if it was empty
		}
	}

	return hit;
}

void VoxelScene::drawPixels(int workload, int x, int y, Window& window, Camera& camera, std::vector<std::vector<glm::vec3>>& buffer, std::atomic<bool>* ticket)
{
	for (int i = 0; i < workload; i++)
	{
		//Compute position and direction of a ray
		float xx = (2.f * ((x + 0.5f) * camera.wdithStep) - 1.f) * camera.angle * camera.aspectratio;
		float yy = (1.f - 2.f * ((y + 0.5f) * camera.heightStep)) * camera.angle;

		glm::vec3 rayDir = glm::normalize(camera.camRot * glm::vec3(xx, yy, 1.f));

		glm::vec3 color;
		glm::vec3 hitPos;
		glm::vec3 normal;

		if (traceRay(worldMap, rayDir, camera.pos, color, hitPos, normal) == true)
		{
			bool hitByLight = false;

			float mDist = -glm::dot(normal, hitPos);

			for (int i = 0; i < pointLights.size() && hitByLight == false; ++i)
			{
				//Check if the on which side of the plane the light is
				//if ((glm::dot(normal, pointLights[i]) + mDist) < 0)
				{
					glm::vec3 filler, filler2;

					rayDir = glm::normalize(glm::vec3(pointLights[i] - hitPos));

					//std::cout << hitPos << std::endl;

					//if (traceRay(worldMap, rayDir, hitPos, color, filler, filler2, &pointLights[i]) == false)
					{
						hitByLight = true;

						//std::cout << hitPos << std::endl;

						float ambientStrength = 0.1f;
						glm::vec3 ambient = lightColor * ambientStrength;
						// diffuse 
						//normal = -normal;
						glm::vec3 lightDir = glm::normalize(pointLights[i] - hitPos);
						float diff = std::max(glm::dot(normal, lightDir), 0.f);
						glm::vec3 diffuse = lightColor * diff;
						// specular
						float specularStrength = 0.5;
						glm::vec3 viewDir = glm::normalize(camera.pos - hitPos);
						glm::vec3 reflectDir = reflect(-lightDir, normal);
						float spec = pow(std::max(dot(viewDir, reflectDir), 0.f), 32.f);
						glm::vec3 specular = lightColor * specularStrength * spec;
						glm::vec3 result = (ambient + diffuse + specular) * color;
						//buffer[x][y] = glm::min(result, glm::vec3(1.f));
						//buffer[x][y] = abs(normal);
						buffer[x][y] = color;
					}
				}
			}

			if (hitByLight == false)
			{
				buffer[x][y] = glm::vec3(0, 0, 0);
			}
			
		}
		else {
			buffer[x][y] = glm::vec3(0, 0, 0);
		}

		if (ticket != nullptr)
		{
			(*ticket) = true;
			ticket->notify_one();
		}

		y++;

		if (y >= camera.windowHeight)
		{
			x++;
			y -= camera.windowHeight;
		}
	}
}


void VoxelScene::intersectPlane(const glm::vec3& normal, const glm::vec3& posPlane, const glm::vec3& posRay, const glm::vec3& rayDir, float& t)
{
	// assuming vectors are all normalized
	float denom = glm::dot(normal, rayDir);

	glm::vec3 dir = posPlane - posRay;
	t = abs(glm::dot(dir, normal) / denom);
}
