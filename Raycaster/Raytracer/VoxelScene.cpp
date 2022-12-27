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

bool VoxelScene::traceRay(glm::vec3 rayDir, glm::vec3 pos, Color& color)
{
	int mapX, mapY, mapZ;//Case in which the ray currently is

					//In which direction we're going for each axis (either +1, or -1)
	int stepX, stepY, stepZ;

	glm::vec3 sideDist;
	glm::vec3 deltaDist;

	//Dans quel case de la carte on se trouve
	mapX = int(pos.x);
	mapY = int(pos.y);
	mapZ = int(pos.z);

	//Distance d'un coté X ou Y vers un coté respectivement X ou Y
	deltaDist.x = fabs(1 / rayDir.x);
	deltaDist.y = fabs(1 / rayDir.y);
	deltaDist.z = fabs(1 / rayDir.z);

	//calculate step and initial sideDist
	if (rayDir.x < 0)
	{
		stepX = -1;
		sideDist.x = (pos.x - mapX) * deltaDist.x;
	}
	else
	{
		stepX = 1;
		sideDist.x = (mapX + 1.0 - pos.x) * deltaDist.x;
	}

	if (rayDir.y < 0)
	{
		stepY = -1;
		sideDist.y = (pos.y - mapY) * deltaDist.y;
	}
	else
	{
		stepY = 1;
		sideDist.y = (mapY + 1.0 - pos.y) * deltaDist.y;
	}

	if (rayDir.z < 0)
	{
		stepZ = -1;
		sideDist.z = (pos.z - mapZ) * deltaDist.z;
	}
	else
	{
		stepZ = 1;
		sideDist.z = (mapZ + 1.0 - pos.z) * deltaDist.z;
	}

	bool hit = false; //was there a wall hit?
	int side;//Axis on which the the voxel was hit

	//Test case par case
	while (hit == false && mapX < mapWidth && mapX >= 0 && mapY < mapDepth && mapY >= 0 && mapZ < mapHeight && mapZ >= 0)
	{
		//Check if ray has hit a wall
		if (worldMap[mapX][mapY][mapZ].isEmpty == false)
		{
			hit = true;
			color = worldMap[mapX][mapY][mapZ].color;
		}
		else {
			//jump to next map square, OR in x-direction, OR in y-direction
			if (sideDist.x < sideDist.y)
			{
				if (sideDist.x < sideDist.z)
				{
					sideDist.x += deltaDist.x;
					mapX += stepX;
					side = 0;
				}
				else {
					sideDist.z += deltaDist.z;
					mapZ += stepZ;
					side = 2;
				}
			}
			else
			{
				if (sideDist.y < sideDist.z)
				{
					sideDist.y += deltaDist.y;
					mapY += stepY;
					side = 1;
				}
				else {
					sideDist.z += deltaDist.z;
					mapZ += stepZ;
					side = 2;
				}
			}
		}
	}

	return hit;
}


void VoxelScene::drawPixel(int x, int y, Window& window, Camera& camera, std::vector<std::vector<Color>>& buffer, std::atomic<bool>* ticket)
{
	//Calcul la position et direction du rayon
	float xx = (2 * ((x + 0.5) * camera.wdithStep) - 1) * camera.angle * camera.aspectratio;
	float yy = (1 - 2 * ((y + 0.5) * camera.heightStep)) * camera.angle;

	glm::vec3 rayDir = camera.camRot * glm::normalize(glm::vec3(xx, yy, 1));

	Color color;

	if (traceRay(rayDir, camera.pos, color) == true)
	{
		//mtx.lock();
		//window.setPixelColor(glm::vec2(x, y), color);
		//mtx.unlock();

		buffer[x][y] = color;
	}
	else {
		buffer[x][y] = Color(0, 0, 0);
	}

	if (ticket != nullptr)
	{
		(*ticket) = true;
		ticket->notify_one();
	}
}



