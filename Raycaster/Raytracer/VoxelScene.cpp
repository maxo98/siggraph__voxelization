#include "VoxelScene.h"

VoxelScene::VoxelScene()
{
	for (int x = 20; x < 24; x++)
	{
		for (int y = 1; y < mapDepth; y++)
		{
			for (int z = 20; z < 24; z++)
			{
				worldMap[x][y][z].isEmpty = false;
				worldMap[x][y][z].color.r = 128;
			}
		}
	}
}

bool VoxelScene::traceRay(const glm::vec3& rayDir, const glm::uvec3& pos, Color& color, glm::uvec3& hitPos, glm::uvec3* destination)
{
	//We should do stuff here later on
	return traceRay(rayDir, glm::vec3(pos), color, hitPos, destination);
}

bool VoxelScene::traceRay(const glm::vec3& rayDir, const glm::vec3& pos, Color& color, glm::uvec3& hitPos, glm::uvec3* destination)
{
	glm::uvec3 map;//Case in which the ray currently is
	map.x = int(pos.x);
	map.y = int(pos.y);
	map.z = int(pos.z);

	hitPos = map;

	//In which direction we're going for each axis (either +1, or -1)
	int stepX, stepY, stepZ;

	glm::vec3 sideDist;
	glm::vec3 deltaDist;

	//In which case of the map we are


	//Distance to another case on each axis
	deltaDist.x = fabs(1 / rayDir.x);
	deltaDist.y = fabs(1 / rayDir.y);
	deltaDist.z = fabs(1 / rayDir.z);

	//calculate step and initial sideDist
	if (rayDir.x < 0)
	{
		stepX = -1;
		sideDist.x = (pos.x - map.x) * deltaDist.x;
	}
	else
	{
		stepX = 1;
		sideDist.x = (map.x + 1.0 - pos.x) * deltaDist.x;
	}

	if (rayDir.y < 0)
	{
		stepY = -1;
		sideDist.y = (pos.y - map.y) * deltaDist.y;
	}
	else
	{
		stepY = 1;
		sideDist.y = (map.y + 1.0 - pos.y) * deltaDist.y;
	}

	if (rayDir.z < 0)
	{
		stepZ = -1;
		sideDist.z = (pos.z - map.z) * deltaDist.z;
	}
	else
	{
		stepZ = 1;
		sideDist.z = (map.z + 1.0 - pos.z) * deltaDist.z;
	}

	bool hit = false; //was there a wall hit?
	int side;//Axis on which the the voxel was hit

	//Test case by case
	while (hit == false && map.x < mapWidth && map.x >= 0 && map.y < mapDepth && map.y >= 0 && map.z < mapHeight && map.z >= 0)
	{
		//Check if ray has hit a wall
		if (worldMap[map.x][map.y][map.z].isEmpty == false)
		{
			hit = true;
			color = worldMap[map.x][map.y][map.z].color;
		}
		else {

			hitPos = map;

			if (destination != nullptr && destination->x == map.x && destination->y == map.y && destination->z == map.z)
			{
				return hit;
			}

			//jump to next map square, OR in x-direction, OR in y-direction
			if (sideDist.x < sideDist.y)
			{
				if (sideDist.x < sideDist.z)
				{
					sideDist.x += deltaDist.x;
					map.x += stepX;
					side = 0;
				}
				else {
					sideDist.z += deltaDist.z;
					map.z += stepZ;
					side = 2;
				}
			}
			else
			{
				if (sideDist.y < sideDist.z)
				{
					sideDist.y += deltaDist.y;
					map.y += stepY;
					side = 1;
				}
				else {
					sideDist.z += deltaDist.z;
					map.z += stepZ;
					side = 2;
				}
			}
		}
	}

	return hit;
}


void VoxelScene::drawPixels(int workload, int x, int y, Window& window, Camera& camera, std::vector<std::vector<Color>>& buffer, std::atomic<bool>* ticket)
{
	for (int i = 0; i < workload; i++)
	{
		//Compute position and direction of a ray
		float xx = (2 * ((x + 0.5) * camera.wdithStep) - 1) * camera.angle * camera.aspectratio;
		float yy = (1 - 2 * ((y + 0.5) * camera.heightStep)) * camera.angle;

		glm::vec3 rayDir = camera.camRot * glm::normalize(glm::vec3(xx, yy, 1));

		Color color;
		glm::uvec3 hitPos;

		if (traceRay(rayDir, camera.pos, color, hitPos) == true)
		{
			bool hitByLight = false;

			for (int i = 0; i < pointLights.size() && hitByLight == false; ++i)
			{
				glm::uvec3 filler;

				rayDir = glm::normalize(glm::vec3(pointLights[i] - hitPos));

				if (traceRay(rayDir, hitPos, color, filler, &pointLights[i]) == false)
				{
					hitByLight = true;
				}
			}

			if (hitByLight == true)
			{
				buffer[x][y] = color;
			}
			else {
				buffer[x][y] = Color(0, 0, 0);
			}
			
		}
		else {
			buffer[x][y] = Color(0, 0, 0);
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



