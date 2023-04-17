#include "Window.h"

#include <SDL.h>
#include <gtx/quaternion.hpp>
#include "VoxelScene.h"
#include "ThreadPool.h"
#include <iomanip>
#include "Genome.h"
#include "Neat.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PROGRAM_NAME "Raycaster V1.0"

#define windowWidth 1920
#define windowHeight 1080

#define MULTITHREAD

#define RADIUS 7

#define OCTSIZE 0.00390625

#define GEN 50


float sceneTest(NeuralNetwork& network, const std::vector<std::vector<float>>& outputs,
	const std::vector<std::vector<bool>>& inputs);

//#define LOAD

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

	auto seed = time(NULL);

	std::cout << "seed " << seed << std::endl;

	srand(seed);//Some random parts don't use this seed 

	Window window("Raycaster V1.0", windowWidth, windowHeight);

	int renderScene = 0;// 4;
	std::vector<VoxelScene*> scenes;
	//scenes.reserve(8);

	//scenes.push_back(new VoxelScene(7));
	scenes.push_back(new VoxelScene(8));
	//scenes.push_back(new VoxelScene(7));
	//scenes.push_back(new VoxelScene(8));
	//scenes.push_back(new VoxelScene(7));
	//scenes.push_back(new VoxelScene(8));
	//scenes.push_back(new VoxelScene(7));
	//scenes.push_back(new VoxelScene(8));

	//scene.addPointLight(glm::vec3(1.75, 3, 0));
	scenes[renderScene]->addPointLight(glm::vec3(3.5, 4, 0));

	//Spaceship
	//2.18164 2.135 2.43359
	//3.81836 2.73461 3.42578

	//doughnut9
	//2.33203 2.06469 2.25977
	//3.69141 2.58617 3.63672

	//Cannon9
	//2.23438 2.02953 2.64453
	//3.76367 3.07836 3.67578

	//scenes[0]->loadModel(glm::dvec3(3, 2.01, 3), "Spaceship7.txt");
	//scenes[1]->loadModel(glm::dvec3(3, 2.01, 3), "Spaceship8.txt");

	//scenes[2]->loadModel(glm::dvec3(3, 2.01, 3), "Cannon7.txt");
	//scenes[3]->loadModel(glm::dvec3(3, 2.01, 3), "Cannon8.txt");

	//scenes[4]->loadModel(glm::dvec3(3, 2.01, 3), "doughnut7.txt");
	//scenes[5]->loadModel(glm::dvec3(3, 2.01, 3), "doughnut8.txt");

	//scenes[6]->loadModel(glm::dvec3(3, 2.01, 3), "Cube7.txt");
	//scenes[7]->loadModel(glm::dvec3(3, 2.01, 3), "Cube8.txt");

	//scenes[0]->loadModel(glm::dvec3(3, 2.01, 3), "Cube7.txt");
	//scenes[1]->loadModel(glm::dvec3(3, 2.01, 3), "Cube8.txt");

	//scenes[0]->loadModel(glm::dvec3(3, 3, 5), "Sphere7.txt");
	scenes[0]->loadModel(glm::dvec3(3, 2, 4.55), "Sphere8.txt");

	std::cout << "Simplifying\n";

	for (int i = 0; i < scenes.size(); i++)
	{
		scenes[i]->simplify();
	}

	std::cout << "Done\n";


	//Camera variables
	glm::vec3 pos(3, 3, 0);  //x and y start position
	float deg = 45;
	glm::quat camRot;

	Camera cam(pos, camRot, 60, windowWidth, windowHeight);
	cam.lookAt(glm::vec3(3, 3, 5));



	//Set node location
	std::vector<std::vector<bool>> inputs;
	std::vector<std::vector<float>> outputs;

	int w;
	int h;
	int comp;
	float* dataValue = stbi_loadf("SphereFrontValue.png", &w, &h, &comp, STBI_rgb_alpha);
	float* dataSign = stbi_loadf("SphereFrontSign.png", &w, &h, &comp, STBI_rgb_alpha);

	int in = 0;
	int out = 0;

	for (int x = 0; x < windowWidth; x++)
	{
		for (int y = 0; y < windowHeight; y++)
		{
			if (*(dataValue + (x + w * y) * comp) != 0.058187179267406463623046875 || *(dataValue + (x + w * y) * comp + 1) != 0.058187179267406463623046875 || *(dataValue + (x + w * y) * comp + 2) != 0.058187179267406463623046875)
			{
				if (scenes[0]->generateData(x, y, cam, inputs, OCTSIZE, RADIUS, in, out) == true)
				{
					glm::vec3 normal = glm::normalize(glm::vec3(*(dataValue + (x + w * y) * comp) * (*(dataSign + (x + w * y) * comp) > 0 ? 1 : -1),
						*(dataValue + (x + w * y) * comp + 1) * (*(dataSign + (x + w * y) * comp + 1) > 0 ? 1 : -1),
						*(dataValue + (x + w * y) * comp + 2) * (*(dataSign + (x + w * y) * comp + 2) > 0 ? 1 : -1)));

					outputs.push_back(std::vector<float>());

					for (int axis = 0; axis < 3; axis++)
					{
						outputs.back().push_back(normal[axis]);
					}
				}
			}
		}
	}

	std::cout << in << " - " << out << std::endl;

	NeuralNetwork network;

#ifndef LOAD

	std::vector<Activation*> arrActiv;
	Activation* tanh = new TanhActivation();
	Activation* linear = new LinearActivation();
	arrActiv.push_back(tanh);

	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> allConn;

	int nInputs = 0;

	glm::dvec3 pointPos;

	double maxDist = RADIUS * OCTSIZE;

	for (double x = -RADIUS; x <= RADIUS; x++)
	{
		pointPos.x = x * OCTSIZE;

		for (double y = -RADIUS; y <= RADIUS; y++)
		{
			pointPos.y = y * OCTSIZE;

			for (double z = -RADIUS; z <= RADIUS; z++)
			{
				pointPos.z = z * OCTSIZE;

				if (maxDist >= glm::length(pointPos))
				{
					nInputs++;
				}
			}
		}
	}

	nInputs++;//Bias

	Genome gen(nInputs, 3, arrActiv);

	gen.fullyConnect(1, 60, tanh, linear, allConn, xavierUniformInit, seed);

	Neat::genomeToNetwork(gen, network);

	//Do test
	int epoch = 50000000;
	float lRate = 0.0000005;

	unsigned int percent = 0;
	unsigned int div = epoch / 100.f;

	if (div == 0) div = 1;

	std::cout << "Training" << std::endl;

	std::cout << "Progress: " << percent << "%" << std::endl;

	std::vector<float> inputsFloat;
	inputsFloat.resize(inputs[0].size()+1);

	inputsFloat.back() = 0.5;

	for (int i = 0; i < epoch; i++)
	{
		int index = randInt(0, inputs.size() - 1);

		for (int i = 0; i < inputsFloat.size(); i++)
		{
			inputsFloat[i] = (inputs[index][i] == true ? 1 : 0);
		}

		network.backprop(inputsFloat, outputs[index], lRate, true);

		if ((i + 1) % div == 0)
		{
			percent++;
			std::cout << "Progress: " << percent << " %\n";
		}
	}

	//Save
	network.applyBackprop(gen);
	gen.saveCurrentGenome("saveGenome.txt");

#else // LOAD
	Genome gen = Genome::loadGenome("saveGenome.txt");

	Neat::genomeToNetwork(gen, network);
#endif 

	sceneTest(network, outputs, inputs);

	//applyResult(&network, scenes, renderScene);

	//Rendering
	//Window window("Raycaster V1.0", windowWidth, windowHeight);

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
				std::vector<NeuralNetwork> networks;

	#ifdef MULTITHREAD
				while (workload < 1)
				{
					cpus--;
					workload = totalWorkload / cpus;
				}

				networks.reserve(cpus);

				while (cpus > threads)
				{
					currentWorkload = floor(workload);
					float workloadFrac = fmod(workload, 1.0f);
					restWorkload = workloadFrac;

					networks.push_back(NeuralNetwork());
					Neat::genomeToNetwork(gen, networks.back());

					tickets.emplace_back(false);
					pool->queueJob(&VoxelScene::drawPixels, scenes[renderScene], currentWorkload, x, y, std::ref(window), std::ref(cam), std::ref(buffer), OCTSIZE, RADIUS, &networks.back(), &tickets.back());
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

				networks.push_back(NeuralNetwork());
				Neat::genomeToNetwork(gen, networks.back());

				scenes[renderScene]->drawPixels(currentWorkload, x, y, window, cam, buffer, OCTSIZE, RADIUS, &networks.back());

				timer = SDL_GetTicks();

				for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
				{
					itTicket->wait(false);
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
	}

	stbi_image_free(dataSign);
	stbi_image_free(dataValue);

#ifndef LOAD
	delete tanh;
	delete linear;
#endif // !LOAD

	for (int i = 0; i < scenes.size(); i++)
	{
		delete scenes[i];
	}

	return 0;
}

float sceneTest(NeuralNetwork& network, const std::vector<std::vector<float>>& outputs,
	const std::vector<std::vector<bool>>& inputs)
{
	std::vector<float> networkOutputs;
	std::vector<float> inputsFloat;

	inputsFloat.resize(inputs[0].size() + 1);
	inputsFloat.back() = 0.5;


	float score = outputs.size();

	for (int cpt = 0; cpt < outputs.size(); cpt++)
	{
		for (int i = 0; i < inputsFloat.size(); i++)
		{
			inputsFloat[i] = (inputs[cpt][i] == true ? 1 : 0);
		}

		network.compute(inputsFloat, networkOutputs);

		//Do test
		float square = 0;

		for (int axis = 0; axis < 3; axis++)
		{
			square += pow(networkOutputs[axis] - outputs[cpt][axis], 2);
		}

		score -= sqrt(square);

	}

	std::cout << score << " / " << outputs.size() << std::endl;

	return score;
}