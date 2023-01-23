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

	(worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree = new Octree<glm::vec3>[8];
	(worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->contains = OCTREE_CONTENT::SPARSE;
	//((worldMap.map + 1 * worldMap.height * worldMap.depth + 1 * worldMap.depth + 3)->tree + 0 * 4 + 0 * 2 + 0)->object = new glm::vec3(0.5, 0, 0);
	//((worldMap.map + 1 * worldMap.height * worldMap.depth + 1 * worldMap.depth + 3)->tree + 0 * 4 + 0 * 2 + 0)->contains = OCTREE_CONTENT::FILLED;

	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 0 * 2 * 2 + 0 * 2 + 0)->object = new glm::vec3(0.5, 0, 0);
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 0 * 2 * 2 + 0 * 2 + 0)->contains = OCTREE_CONTENT::FILLED;
	//((worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 2)->object = new glm::vec3(0, 0.5, 0);
	//((worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 2)->contains = OCTREE_CONTENT::FILLED;
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 2)->object = new glm::vec3(0, 0, 0.5);
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 2)->contains = OCTREE_CONTENT::FILLED;
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 3)->object = new glm::vec3(0.5, 0.5, 0.5);
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 3)->contains = OCTREE_CONTENT::FILLED;
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 4)->object = new glm::vec3(0.5, 0.5, 0);
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 4)->contains = OCTREE_CONTENT::FILLED;
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 5)->object = new glm::vec3(0, 0.5, 0.5);
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 5)->contains = OCTREE_CONTENT::FILLED;
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 6)->object = new glm::vec3(0.5, 0, 0.5);
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 6)->contains = OCTREE_CONTENT::FILLED;
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 7)->object = new glm::vec3(1, 1, 1);
	//((worldMap.map + 0 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 7)->contains = OCTREE_CONTENT::FILLED;

	((worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 2)->tree = new Octree<glm::vec3>[8];
	((worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 2)->contains = OCTREE_CONTENT::SPARSE;
	(((worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 2)->tree + 0)->object = new glm::vec3(0.3, 0.3, 0.3);
	(((worldMap.map + 1 * worldMap.height * worldMap.depth + 0 * worldMap.depth + 3)->tree + 2)->tree + 0)->contains = OCTREE_CONTENT::FILLED;



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

/*
unsigned char a = 0;

void rayParam(Octree<glm::vec3>& oct, glm::vec3 rayDir, glm::vec3 pos, float level)
{
	

	if (rayDir.x < 0.0f)
	{
		pos.x = level * 2 - pos.x;
		rayDir.x = -rayDir.x;
		a |= 4;
	}

	if (rayDir.y < 0.0f)
	{
		pos.y = level * 2 - pos.y;
		rayDir.y = -rayDir.y;
		a |= 2;
	}

	if (rayDir.z < 0.0f)
	{
		pos.z = level * 2 - pos.z;
		rayDir.z = -rayDir.z;
		a |= 1;
	}

	float tx0 = (oct->xmin - r.ox) / r.dx;
	float tx1 = (oct->xmax - r.ox) / r.dx;
	float ty0 = (oct->ymin - r.oy) / r.dy;
	float ty1 = (oct->ymax - r.oy) / r.dy;
	float tz0 = (oct->zmin - r.oz) / r.dz;
	float tz1 = (oct->zmax - r.oz) / r.dz;

	if (Max(tx0, ty0, tz0) < Min(tx1, ty1, tz1))
	{
		procSubtree(tx0, ty0, tz0, tx1, ty1, tz1, oct->root)
	}
}

void procSubtree(float tx0, float ty0, float tz0, float tx1, float ty1, float tz1, node* n)
{
	float txm, tym, tzm;
	int currNode;

	if (tx1 < 0.0f || ty1 < 0.0f || tz1 < 0.0f)
		return;

	if (n->type == TERMINAL)
	{
		proc_terminal(n);
		return;
	}

	txm = 0.5 * (tx0 + tx1);
	tym = 0.5 * (ty0 + ty1);
	tym = 0.5 * (tz0 + tz1);

	currNode = firstNode(tx0)
}*/

//NOTE: when we get out of the bounds of the map by substracting 1, it will take the maximum value of an unsigned int
//which should be fine as long as the voxel map size is not equal to the max value of an unsigned int on any of the axis
//NOTE2: hitPos needs to be fixed
bool VoxelScene::traceRay(VoxelMap& map, const glm::vec3& rayDir, const glm::vec3& pos, glm::vec3& color, glm::vec3& hitPos,
	glm::vec3& normal, glm::vec3* destination, float lvl, MapEntrance* entrance)
{
	glm::uvec3 mapPos;//Case in which the ray currently is
	glm::vec3 gridPos;//Worldpos of the voxel, used to know how we have traveled (in euclidean distance)
	float sqrLvl = lvl/2 * lvl/2;

	glm::uvec3 test;

	Axis side = Axis::X;//Axis on which the the voxel was hit

	//When entering the grid
	if (entrance == nullptr)
	{
		//Currently assumes we are in the grid at the start
		mapPos.x = round(pos.x);
		mapPos.y = round(pos.y);
		mapPos.z = round(pos.z);

		gridPos = mapPos;
	}
	else {//When entering an octree

		float t;

		gridPos = entrance->pos;

		if (intersectPlane(entrance->normal, gridPos, pos, rayDir, t) == false)
		{
			std::cout << "Error entering octree\n";//We need to handle case in which we start inside the octree
			return false;
		}

		//if (lvl == 0.25f)
		//{
		//	std::cout << gridPos << std::endl;
		//}
		
		glm::vec3 intersect = rayDir * (t - std::numeric_limits<float>::epsilon() * 100) + pos - gridPos;//Get intersect pos relative to the octree

		for (uint8_t i = 0; i < 3; ++i)
		{
			if (entrance->normal[i] == 0)
			{
				if (lvl == 0.5f)
				{
					//std::cout << intersect[i] << std::endl;
				}

				if (intersect[i] < lvl)
				{
					mapPos[i] = 0;
					gridPos[i] -= lvl;
				}
				else {
					mapPos[i] = 1;
					gridPos[i] += lvl;
				}
			}
			else {
				side = (Axis)i;

				if (entrance->normal[i] == 1)
				{
					mapPos[i] = 0;
					gridPos[i] -= lvl;
				}
				else if (entrance->normal[i] == -1)
				{
					mapPos[i] = 1;
					gridPos[i] += lvl;
				}
			}
		}

		test = mapPos;
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
					gridPos[i] -= halfLvl * (step[i] - std::numeric_limits<float>::epsilon());
					normal[i] = step[i];
				}
			}

			if (intersectPlane(normal, gridPos, pos, rayDir, t) == false)
			{
				std::cout << "Error entering octree 2\n" << std::endl;// << gridPos << " " << normal << std::endl << pos << rayDir;
				//return false;
			}			
			
			//std::cout << mapPos << std::endl;

			//std::cout << lvl << std::endl;
			
			hitPos = rayDir * (t - std::numeric_limits<float>::epsilon()*100) + pos;//Get intersect pos relative to the octree
		}
		else//Sparse octree
		{
			float newLvl = lvl / 2;
			MapEntrance newEntrance;

			newEntrance.pos = gridPos;

			for (uint8_t i = 0; i < 3; ++i)
			{
				if (i != (uint8_t)side)
				{
					newEntrance.normal[i] = 0;
				}
				else {
					newEntrance.normal[i] = step[i];
					//newEntrance.pos[i] -= newLvl * step[i];
				}
			}

			VoxelMap map;

			map.map = currentTree->tree;
			map.depth = 2;
			map.height = 2;
			map.width = 2;

			hit = traceRay(map, rayDir, pos, color, hitPos, normal, destination, newLvl, &newEntrance);

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
						normal = -normal;
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
						buffer[x][y] = glm::min(result, glm::vec3(1.f));
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



