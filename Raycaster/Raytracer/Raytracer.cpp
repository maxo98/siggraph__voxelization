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

void pollEvents(Window &_window, SDL_Event &_keyboard, int &_mouseX, int &_mouseY) {//Input
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		_window.pollEvents(event);
		_keyboard = event;
		
		if(event.type == SDL_MOUSEMOTION)
		{
			_mouseX = event.motion.xrel;
			_mouseY = event.motion.yrel;
		}
	}
}



int main(int argc, char *argv[])
{
	ThreadPool* pool = ThreadPool::getInstance();
	pool->start();

	VoxelScene scene;

	//scene.addPointLight(glm::vec3(1.75, 3, 0));
	scene.addPointLight(glm::vec3(3.5, 4, 0));

	Window window("Raycaster V1.0", windowWidth, windowHeight);

	window.clear();

	int timer = 0;
	int i, cpt;

	int uniformLocation;

	SDL_Event keyboard;///////////Input variables
	int lastMouseX = 0;
	int lastMouseY = 0;
	int mouseX = 0;
	int mouseY = 0;

	float mouvSpeed = 0.1;

	//Camera variables
	glm::vec3 pos(3, 5, 0);  //x and y start position
	float deg = 45;
	glm::quat camRot;

	Camera cam(pos, camRot, 60, windowWidth, windowHeight);
	cam.lookAt(glm::vec3(3, 3, 3));
	
	//SDL_ShowCursor(SDL_DISABLE);////////Options fenêtre SDL
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	//SDL_SetWindowGrab(window.getWindow(), SDL_TRUE);
	//SDL_SetRelativeMouseMode(SDL_TRUE);
	//SDL_WarpMouseInWindow(window.getWindow(), windowWidth/2, windowHeight/2);

	std::vector<std::vector<glm::vec3>> buffer;

	buffer.resize(windowWidth, std::vector<glm::vec3>(windowHeight, glm::vec3(0,0,1)));

	bool first = true;

	while (!window.isClosed())///////////////////////////////////////////////////////////////////////////////Main loop
	{
		pollEvents(window, keyboard, mouseX, mouseY);///////////////////////Keyboard input
		if(SDLK_b && keyboard.type == SDL_KEYDOWN)
		{
			switch(keyboard.key.keysym.sym)
			{
				case SDLK_z:
					cam.pos.z += 1;
					break;
				case SDLK_q:
					cam.pos.x -= 1;
					break;
				case SDLK_s:
					cam.pos.z -= 1;
					break;
				case SDLK_d:
					cam.pos.x += 1;
					break;
			}
			//std::cout << "position x : " << cam.pos.x << " position z : " << cam.pos.z << std::endl;
		}

		if (timer + 42 < SDL_GetTicks())////////////////////Display
		{
			//window.clear();
			//SDL_GetRelativeMouseState(&mouseX, &mouseY);
			//std::cout << "mouse x : " << mouseX << " mouse y : " << mouseY << std::endl;
			if (lastMouseX != mouseX && lastMouseY != mouseY)//////Camera rotation
			{
				//
				lastMouseX = mouseX;
				lastMouseY = mouseY;
				//cam.rotateCamera(mouseX, mouseY, mouvSpeed);
				//SDL_WarpMouseInWindow(window.getWindow(), windowWidth/2, windowHeight/2);
			}

			if(first == true)
			{
				first = false;

				int threads = 1;
				unsigned int cpus = std::thread::hardware_concurrency() - 1;

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
					pool->queueJob(&VoxelScene::drawPixels, &scene, currentWorkload, x, y, std::ref(window), std::ref(cam), std::ref(buffer), &tickets.back());
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

				scene.drawPixels(currentWorkload, x, y, window, cam, buffer);

				timer = SDL_GetTicks();

				for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
				{
					itTicket->wait(false);
				}
			}

			//Window pixel color
			for (int x = 0; x < windowWidth; x++)
			{
				for (int y = 0; y < windowHeight; y++)
				{
					window.setPixelColor(glm::vec2(x, y), buffer[x][y]);
				}
			}

			SDL_RenderPresent(Window::renderer);
		}
	}

	return 0;
}



