#include "Window.h"

#include <math.h>

#include <SDL.h>
#include <gtx/quaternion.hpp>
 
#define PI 3.14159265

#define PROGRAM_NAME "Raycaster V1.0"

#define mapWidth 24
#define mapHeight 24
#define mapDepth 3

#define windowWidth 1920
#define windowHeight 1080

//const int worldMap[mapWidth][mapHeight][mapDepth]=
//{
//  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
//  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
//  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
//};

int worldMap[mapWidth][mapDepth][mapHeight] = {0};

const float rouge[4] = { 1.0f, 0.0, 0.0, 1.0 };
const float vert[4] = { 0.0, 1.0, 0.0, 1.0};
const float bleu[4] = { 0.0, 0.0, 1.0, 1.0 };

void pollEvents(Window &_window, SDL_Event &_keyboard, int &_mouse) {//Input
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		_window.pollEvents(event);
		_keyboard = event;
		
		if(event.type == SDL_MOUSEMOTION)
		{
			_mouse = event.motion.x;
		}
	}
}



int main(int argc, char *argv[])
{
	Window window("Raycaster V1.0", windowWidth, windowHeight);

	window.clear();

	int timer = 0;
	int i, cpt;

	int uniformLocation;

	SDL_Event keyboard;///////////Variables pour la lecture d'entré
	int lastMouse = 0;
	int mouse = 0;

	float mouvSpeed = 0.3;
	
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapDepth; y++)
		{
			for (int z = 0; z < mapHeight; z++)
			{
				worldMap[x][y][z] = 0;
			}
		}
	}


	for (int x = 20; x < 25; x++)
	{
		for (int y = 1; y < mapDepth; y++)
		{
			for (int z = 20; z < 25; z++)
			{
				worldMap[x][y][z] = 1;
			}
		}
	}

	glm::vec3 pos(22, 0, 12);  //x and y start position
	
	//Camera variables
	float heightStep = 1 / (float)windowHeight;
	float wdithStep = 1 / (float)windowWidth;
	float fov = 30, aspectratio = windowWidth / float(windowHeight);
	float angle = tan(M_PI * 0.5 * fov / 180.);
	glm::quat camRot;

	SDL_ShowCursor(SDL_DISABLE);////////Options fenêtre SDL
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_SetWindowGrab(window.getWindow(), SDL_TRUE);
	SDL_WarpMouseInWindow(window.getWindow(), windowWidth/2, windowHeight/2);

	while (!window.isClosed())///////////////////////////////////////////////////////////////////////////////Main loop
	{
		pollEvents(window, keyboard, mouse);///////////////////////Entré Clavier
		if(SDLK_b && keyboard.type == SDL_KEYDOWN)
		{
			//switch(keyboard.key.keysym.sym)
			//{
			//	case SDLK_z:
			//		if(worldMap[int(pos.x + camRot * mouvSpeed)][int(pos.z)][0] == false) pos.x += dirX * mouvSpeed;
	  //    			if(worldMap[int(pos.x)][int(pos.y + dirY * mouvSpeed)][0] == false) pos.y += dirY * mouvSpeed;
			//		break;
			//	case SDLK_q:
			//		if(worldMap[int(pos.x - planeX * mouvSpeed)][int(pos.y)][0] == false) pos.x -= planeX * mouvSpeed;
	  //    			if(worldMap[int(pos.x)][int(pos.y - planeY * mouvSpeed)][0] == false) pos.y -= planeY * mouvSpeed;
			//		break;
			//	case SDLK_s:
			//		if(worldMap[int(pos.x - dirX * mouvSpeed)][int(pos.y)][0] == false) pos.x -= dirX * mouvSpeed;
   //   				if(worldMap[int(pos.x)][int(pos.x - dirY * mouvSpeed)][0] == false) pos.y -= dirY * mouvSpeed;
			//		break;
			//	case SDLK_d:
			//		if(worldMap[int(pos.x + planeX * mouvSpeed)][int(pos.y)][0] == false) pos.x += planeX * mouvSpeed;
	  //    			if(worldMap[int(pos.x)][int(pos.y + planeY * mouvSpeed)][0] == false) pos.y += planeY * mouvSpeed;
			//		break;
			//}
		}

		if(timer + 42 < SDL_GetTicks())////////////////////Affichage
		{
			window.clear();

			if(lastMouse != mouse)//////Rotation Camera
			{



				SDL_WarpMouseInWindow(window.getWindow(), windowWidth/2, windowHeight/2);
				
			}

			//sol.renderFill();
			for(int x = 0; x < windowWidth; x++)/////////////////////////Raycasting
			{
				for (int y = 0; y < windowHeight; y++)
				{
					//Calcul la position et direction du rayon
					float xx = (2 * ((x + 0.5) * wdithStep) - 1) * angle * aspectratio;
					float yy = (1 - 2 * ((y + 0.5) * heightStep)) * angle;

					glm::vec3 rayDir = camRot * glm::normalize(glm::vec3(xx, yy, 1));

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

						//Check if ray has hit a wall
						if (worldMap[mapX][mapY][mapZ] > 0) hit = true;
					}

					if (hit == false) continue;

					window.setPixelColor(glm::vec2(x, y), glm::vec3(255, 0, 0));
				}
			}

			timer = SDL_GetTicks();
			
			SDL_RenderPresent(Window::renderer);
		}
	}

	return 0;
}



