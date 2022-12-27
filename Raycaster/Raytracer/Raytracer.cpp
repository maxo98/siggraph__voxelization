#include "Window.h"

#include <SDL.h>
#include <gtx/quaternion.hpp>
#include "VoxelScene.h"
#include "ThreadPool.h"
#include <iomanip>

#define PROGRAM_NAME "Raycaster V1.0"

#define windowWidth 1920
#define windowHeight 1080

#define MULTITHREAD

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
	ThreadPool* pool = ThreadPool::getInstance();
	pool->start();

	Window window("Raycaster V1.0", windowWidth, windowHeight);

	window.clear();

	int timer = 0;
	int i, cpt;

	int uniformLocation;

	SDL_Event keyboard;///////////Variables pour la lecture d'entré
	int lastMouse = 0;
	int mouse = 0;

	float mouvSpeed = 0.3;

	//Camera variables
	glm::vec3 pos(22, 0, 12);  //x and y start position
	glm::quat camRot;

	Camera cam(pos, camRot, 30, windowWidth, windowHeight);

	SDL_ShowCursor(SDL_DISABLE);////////Options fenêtre SDL
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_SetWindowGrab(window.getWindow(), SDL_TRUE);
	SDL_WarpMouseInWindow(window.getWindow(), windowWidth/2, windowHeight/2);

	VoxelScene scene;
	std::vector<std::vector<Color>> buffer;

	buffer.resize(windowWidth, std::vector<Color>(windowHeight, Color(0,0,255)));

	bool first = true;


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

			time_t start, end;

			if (first == true)
			{
				time(&start);
				std::ios_base::sync_with_stdio(false);
			}

			//std::deque<std::atomic<bool>> tickets;

			//for(int x = 0; x < windowWidth; x++)/////////////////////////Raycasting
			//{
			//	for (int y = 0; y < windowHeight; y++)
			//	{
			//		//tickets.emplace_back(false);

			//		//pool->queueJob(&VoxelScene::drawPixel, &scene, x, y, std::ref(window), std::ref(cam), std::ref(buffer), &tickets.back());
			//	}
			//}

			int threads = 1;
			unsigned int cpus = std::thread::hardware_concurrency();

			float totalWorkload = windowWidth * windowHeight;
			float workload = totalWorkload / cpus;
			float restWorkload = 0;
			int currentWorkload = totalWorkload;
			int startIndex = 0;
			int count = 0;

			std::deque<std::atomic<bool>> tickets;

			int x = 0, y = 0;

#ifdef MULTITHREAD
			while (workload < 1)
			{
				cpus--;
				workload = totalWorkload / cpus;
			}

			while (cpus > threads)
			{
				currentWorkload = floor(workload);
				float workloadFrac = fmod(workload, 1.0f);
				restWorkload = workloadFrac;

				tickets.emplace_back(false);
				pool->queueJob(&VoxelScene::drawPixel, &scene, currentWorkload, x, y, std::ref(window), std::ref(cam), std::ref(buffer), &tickets.back());
				++threads;

				x += currentWorkload / windowHeight;
				y += fmod(currentWorkload / float(windowHeight), 1.0f) * windowHeight;

				if (y >= windowHeight)
				{
					x++;
					y -= windowHeight;
				}

				count += currentWorkload + floor(restWorkload);
				startIndex += currentWorkload + floor(restWorkload);

				restWorkload -= floor(restWorkload);
				restWorkload += workloadFrac;
			}

			while (restWorkload > 0)
			{
				restWorkload--;
				currentWorkload++;
			}
#endif // MULTITHREAD

			count += currentWorkload;

			while (count > totalWorkload)
			{
				currentWorkload--;
				count--;
			}

			scene.drawPixel(currentWorkload, x, y, window, cam, buffer);

			timer = SDL_GetTicks();

			for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
			{
				itTicket->wait(false);
			}

			for (int x = 0; x < windowWidth; x++)/////////////////////////Raycasting
			{
				for (int y = 0; y < windowHeight; y++)
				{
					window.setPixelColor(glm::vec2(x, y), buffer[x][y]);
				}
			}

			SDL_RenderPresent(Window::renderer);

			if (first == true)
			{
				first = false;

				time(&end);

				// Calculating total time taken by the program.
				double time_taken = double(end - start);
				std::cout << "Time taken by program is : " << std::fixed
					<< time_taken << std::setprecision(5);
				std::cout << " sec " << std::endl;
			}
		}
	}

	return 0;
}



