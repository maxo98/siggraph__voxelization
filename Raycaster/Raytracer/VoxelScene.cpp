#include "VoxelScene.h"

VoxelScene::VoxelScene()
{
	for (int x = 20; x < 24; x++)
	{
		for (int y = 1; y < 3; y++)
		{
			for (int z = 20; z < 24; z++)
			{
				worldMap[x][y][z].isEmpty = false;
				worldMap[x][y][z].color.r = 128;
			}
		}
	}
}

bool VoxelScene::traceRay(const glm::vec3& rayDir, const glm::uvec3& pos, Color& color, glm::uvec3& hitMapPos, glm::uvec3* destination)
{
	//We should do stuff here later on
	return traceRay(rayDir, glm::vec3(pos), pos, color, hitMapPos, destination);
}

bool VoxelScene::traceRay(const glm::vec3& rayDir, const glm::vec3& pos, Color& color, glm::uvec3& hitMapPos, glm::uvec3* destination)
{
	//We should do more stuff here later on

	glm::uvec3 mapPos;//Case in which the ray currently is
	mapPos.x = int(pos.x);
	mapPos.y = int(pos.y);
	mapPos.z = int(pos.z);

	return traceRay(rayDir, pos, mapPos, color, hitMapPos, destination);
}


//NOTE: when get out of the bounds of the map by substracting 1, it will take the maximum value of an unsigned int
//which should be fine as long as the voxel map size is not equal to the max value of an unsigned int on any of the axis
//NOTE2: hitMapPos is the gridpos before we hit a voxel
bool VoxelScene::traceRay(const glm::vec3& rayDir, const glm::vec3& pos, glm::uvec3 mapPos, Color& color, glm::uvec3& hitMapPos, glm::uvec3* destination)
{
	hitMapPos = mapPos;

	//In which direction we're going for each axis (either +1, or -1)
	glm::ivec3 step;

	glm::vec3 sideDist;
	glm::vec3 deltaDist;

	//Distance to another case on each axis
	deltaDist.x = fabs(1 / rayDir.x);
	deltaDist.y = fabs(1 / rayDir.y);
	deltaDist.z = fabs(1 / rayDir.z);

	//calculate step and initial sideDist
	if (rayDir.x < 0)
	{
		step.x = -1;
		sideDist.x = (pos.x - mapPos.x) * deltaDist.x;
	}
	else
	{
		step.x = 1;
		sideDist.x = (mapPos.x + 1.0 - pos.x) * deltaDist.x;
	}

	if (rayDir.y < 0)
	{
		step.y = -1;
		sideDist.y = (pos.y - mapPos.y) * deltaDist.y;
	}
	else
	{
		step.y = 1;
		sideDist.y = (mapPos.y + 1.0 - pos.y) * deltaDist.y;
	}

	if (rayDir.z < 0)
	{
		step.z = -1;
		sideDist.z = (pos.z - mapPos.z) * deltaDist.z;
	}
	else
	{
		step.z = 1;
		sideDist.z = (mapPos.z + 1.0 - pos.z) * deltaDist.z;
	}

	bool hit = false; //was there a wall hit?
	Axis side = Axis::X;//Axis on which the the voxel was hit

	//Test case by case
	while (hit == false && mapPos.x < mapWidth && mapPos.x >= 0 && mapPos.y < mapDepth && mapPos.y >= 0 && mapPos.z < mapHeight && mapPos.z >= 0)
	{
		//Check if ray has hit a wall
		if (worldMap[mapPos.x][mapPos.y][mapPos.z].isEmpty == false)
		{
			hit = true;
			color = worldMap[mapPos.x][mapPos.y][mapPos.z].color;
		}
		else {

			hitMapPos = mapPos;

			if (destination != nullptr && destination->x == mapPos.x && destination->y == mapPos.y && destination->z == mapPos.z)
			{
				break;
			}

			//jump to next map square, OR in x-direction, OR in y-direction
			if (sideDist.x < sideDist.y)
			{
				if (sideDist.x < sideDist.z)
				{
					sideDist.x += deltaDist.x;
					mapPos.x += step.x;
					side = Axis::X;
				}
				else {
					sideDist.z += deltaDist.z;
					mapPos.z += step.z;
					side = Axis::Z;
				}
			}
			else
			{
				if (sideDist.y < sideDist.z)
				{
					sideDist.y += deltaDist.y;
					mapPos.y += step.y;
					side = Axis::Y;
				}
				else {
					sideDist.z += deltaDist.z;
					mapPos.z += step.z;
					side = Axis::Z;
				}
			}
		}
	}

	//NOTE:Attempt at a cheap way to compute the exact position, which prooved to be usually wrong/inacurate
	//We could compute plane intersection, be it's a little more expensive
	//We are not using octrees currently so the current compution of "collision point" is inexpensive

	//float t = 0;

	//if (sideDist.x < sideDist.y)
	//{
	//	if (sideDist.x < sideDist.z)
	//	{
	//		t = sideDist.x / rayDir.x;
	//	}
	//	else {
	//		t = sideDist.z / rayDir.z;
	//	}
	//}
	//else {
	//	if (sideDist.y < sideDist.z)
	//	{
	//		t = sideDist.y / rayDir.y;
	//	}
	//	else {
	//		t = sideDist.z / rayDir.z;
	//	}
	//}

	//switch (side)
	//{
	//case Axis::X:
	//	t = sideDist.x / rayDir.x;
	//	std::cout << "x" << std::endl;
	//	break;
	//case Axis::Y:
	//	t = sideDist.y / rayDir.y;
	//	std::cout << "y" << std::endl;
	//	break;
	//case Axis::Z:
	//	t = sideDist.z / rayDir.z;
	//	std::cout << "z" << std::endl;
	//	break;
	//}

	//hitPos = pos + t * rayDir;

	//std::cout << hitPos << " " << hitMapPos << std::endl;

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
		glm::uvec3 hitMapPos;

		if (traceRay(rayDir, camera.pos, color, hitMapPos) == true)
		{
			bool hitByLight = false;

			for (int i = 0; i < pointLights.size() && hitByLight == false; ++i)
			{
				glm::uvec3 filler;

				rayDir = glm::normalize(glm::vec3(pointLights[i] - hitMapPos));

				if (traceRay(rayDir, hitMapPos, color, filler, &pointLights[i]) == false)
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



