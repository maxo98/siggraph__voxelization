#include "Window.h"

#include <SDL.h>
#include <gtx/quaternion.hpp>
#include "VoxelScene.h"
#include "ThreadPool.h"
#include <iomanip>
#include "Hyperneat.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PROGRAM_NAME "Raycaster V1.0"

#define windowWidth 1920
#define windowHeight 1080

#define MULTITHREAD

#define RADIUS 7

#define OCTSIZE 0.00390625

#define GEN 20

bool hypeneatTest(int popSize, Hyperneat* algo, std::vector<glm::vec3>& outputs,
	std::vector<std::vector<bool>>& inputs, std::vector<std::vector<NeuralNetwork>>& networks,
	std::vector<std::vector<std::vector<float>>> inputSubstrate,
	std::vector<std::vector<std::vector<float>>> hiddenSubstrate,
	std::vector<std::vector<float>> outputSubstrate);

void evaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, Hyperneat* algo, const std::vector<glm::vec3>& outputs,
	const std::vector<std::vector<bool>>& inputs, std::vector<std::vector<NeuralNetwork>>& networks, std::atomic<bool>* ticket = nullptr);

float sceneTest(std::vector<std::vector<NeuralNetwork>>& networks, int index, const std::vector<glm::vec3>& outputs,
	const std::vector<std::vector<bool>>& inputs, Hyperneat* algo);

//#define LOAD

std::vector<float> normalEstimationCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	p1.push_back(0.5f);
	return p1;
}

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
	scenes[renderScene]->addPointLight(glm::vec3(3.5, 3, 0));

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


	NeatParameters neatparam;

	neatparam.activationFunctions.push_back(new ThresholdActivation());
	neatparam.activationFunctions.push_back(new TanhActivation());
	neatparam.activationFunctions.push_back(new SinActivation());
	neatparam.activationFunctions.push_back(new GaussianActivation());
	neatparam.activationFunctions.push_back(new LinearActivation());
	//neatparam.activationFunctions.push_back(new AbsActivation());

	neatparam.pbMutateLink = 0.15;// 0.05;
	neatparam.pbMutateNode = 0.09;//0.03;
	//neatparam.pbWeightShift = 0.7;
	//neatparam.pbWeightRandom = 0.2;
	neatparam.pbWeight = 0.9;// 0.9;
	neatparam.pbToggleLink = 0.005;// 0.05;
	//neatparam.weightShiftStrength = 2.5;
	//neatparam.weightRandomStrength = 2.5;
	neatparam.weightMuteStrength = 1.5;// 2.5;
	neatparam.pbMutateActivation = 0.7;

	neatparam.disjointCoeff = 1.0;
	neatparam.excessCoeff = 1.0;
	neatparam.mutDiffCoeff = 0.4;
	neatparam.activationDiffCoeff = 1.0;
	neatparam.weightCoeff = 0;

	neatparam.killRate = 0.2;

	neatparam.champFileSave = "champ";
	neatparam.avgFileSave = "avg";//Without extension type file
	neatparam.saveChampHistory = true;
	neatparam.saveAvgHistory = true;

	neatparam.pbMateMultipoint = 0.6;
	neatparam.pbMateSinglepoint = 0.0;
	neatparam.interspeciesMateRate = 0.001;
	neatparam.dropOffAge = 15;
	neatparam.ageSignificance = 1.0;
	neatparam.pbMutateOnly = 0.25;
	neatparam.pbMateOnly = 0.2;

	neatparam.speciationDistance = 3.0;


	neatparam.speciationDistanceMod = 0.3;
	neatparam.minExpectedSpecies = 4;
	neatparam.maxExpectedSpecies = 8;
	neatparam.adaptSpeciation = true;

	neatparam.keepChamp = true;
	neatparam.elistism = false;
	neatparam.rouletteMultiplier = 2.0;

	HyperneatParameters hyperneatParam;

	hyperneatParam.activationFunction = new LinearActivation();
	hyperneatParam.cppnInput = 2;
	hyperneatParam.cppnInputFunction = normalEstimationCppnInput;
	hyperneatParam.cppnOutput = 1;
	hyperneatParam.nDimensions = 1;
	hyperneatParam.thresholdFunction = noThreshold;
	hyperneatParam.weightModifierFunction = noChangeWeight;

	int popSize = 150;
	int result = 0;
	int count = 0;

	Hyperneat hyper(popSize, neatparam, hyperneatParam, Neat::INIT::ONE);

	//Set node location
	std::vector<std::vector<bool>> inputs;
	std::vector<glm::vec3> outputs;

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
					outputs.push_back(glm::normalize(glm::vec3(*(dataValue + (x + w * y) * comp) * (*(dataSign + (x + w * y) * comp) > 0 ? 1 : -1),
						*(dataValue + (x + w * y) * comp + 1) * (*(dataSign + (x + w * y) * comp + 1) > 0 ? 1 : -1),
						*(dataValue + (x + w * y) * comp + 2) * (*(dataSign + (x + w * y) * comp + 2) > 0 ? 1 : -1))));
				}
			}
		}
	}

	std::cout << in << " - " << out << std::endl;

#ifndef LOAD

	std::vector<std::vector<std::vector<float>>> inputSubstrate;
	std::vector<std::vector<std::vector<float>>> hiddenSubstrate;
	std::vector<std::vector<float>> outputSubstrate;
	outputSubstrate.push_back(std::vector<float>());
	outputSubstrate[0].push_back(0);

	for (int axis = 0; axis < 3; axis++)
	{
		inputSubstrate.push_back(std::vector<std::vector<float>>());
		
	}

	glm::dvec3 inputNetwork;
	glm::dvec3 pointPos;

	double maxDistPlus = (RADIUS + 1) * OCTSIZE;
	double maxDist = RADIUS * OCTSIZE;

	for (double x = -RADIUS; x <= RADIUS; x++)
	{
		inputNetwork.x = (1 - abs(x * OCTSIZE) / maxDistPlus) * (x < 0 ? -1 : 1);
		pointPos.x = x * OCTSIZE;

		for (double y = -RADIUS; y <= RADIUS; y++)
		{
			inputNetwork.y = (1 - abs(y * OCTSIZE) / maxDistPlus) * (y < 0 ? -1 : 1);
			pointPos.y = y * OCTSIZE;

			for (double z = -RADIUS; z <= RADIUS; z++)
			{
				inputNetwork.z = (1 - abs(z * OCTSIZE) / maxDistPlus) * (z < 0 ? -1 : 1);
				pointPos.z = z * OCTSIZE;

				if (maxDist >= glm::length(pointPos))
				{
					for (int axis = 0; axis < 3; axis++)
					{
						inputSubstrate[axis].push_back(std::vector<float>());
						inputSubstrate[axis].back().push_back(inputNetwork[axis]);
					}
				}
			}
		}
	}

	//std::cout << glm::normalize(test) << std::endl;

	//Axis/CPPNS
	std::vector<std::vector<NeuralNetwork>> networks;

	networks.resize(3);
	for (int axis = 0; axis < 3; axis++)
	{
		networks[axis].resize(popSize);

		hyper.initNetworks(networks[axis], inputSubstrate[axis], outputSubstrate, hiddenSubstrate);

		hyper.generateNetworks(networks[axis], inputSubstrate[axis], outputSubstrate, hiddenSubstrate);
	}

	//Do test
	hypeneatTest(popSize, &hyper, outputs, inputs, networks, inputSubstrate, hiddenSubstrate, outputSubstrate);

	//Save
	hyper.saveHistory();


#endif // !LOAD

	//Apply to check result
	Genome* genP;

#ifndef LOAD
	genP = hyper.getGoat();
#else
	Genome gen = Genome::loadGenome("saveGenome.txt");

	genP = &gen;
#endif // !LOAD

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
					pool->queueJob(&VoxelScene::drawPixels, scenes[renderScene], currentWorkload, x, y, std::ref(window), std::ref(cam), std::ref(buffer), OCTSIZE, RADIUS, &hyper, genP, &tickets.back());
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

				scenes[renderScene]->drawPixels(currentWorkload, x, y, window, cam, buffer, OCTSIZE, RADIUS, &hyper, genP);

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

	for (int i = 0; i < neatparam.activationFunctions.size(); i++)
	{
		delete neatparam.activationFunctions[i];
	}

	delete hyperneatParam.activationFunction;

	for (int i = 0; i < scenes.size(); i++)
	{
		delete scenes[i];
	}

	return 0;
}

bool hypeneatTest(int popSize, Hyperneat* algo, std::vector<glm::vec3>& outputs,
	std::vector<std::vector<bool>>& inputs, std::vector<std::vector<NeuralNetwork>>& networks,
	std::vector<std::vector<std::vector<float>>> inputSubstrate,
	std::vector<std::vector<std::vector<float>>> hiddenSubstrate,
	std::vector<std::vector<float>> outputSubstrate)
{
	std::vector<float> fitness;

	fitness.resize(popSize);

	bool validated = false;

	for (int i3 = 0; i3 < GEN && validated == false; i3++)
	{
		std::cout << std::endl << "gen " << i3 << std::endl;

		for (int i = 0; i < popSize; i++)
		{
			fitness[i] = 0;
		}

		int threads = 1;
		ThreadPool* pool = ThreadPool::getInstance();
		size_t taskLaunched = pool->getTasksTotal();
		unsigned int cpus = (pool->getThreadPoolSize() >= taskLaunched ? pool->getThreadPoolSize() - taskLaunched : 0);

		float totalWorkload = popSize;
		float workload = (cpus > 1 ? totalWorkload / cpus : totalWorkload);
		float restWorkload = 0;
		int currentWorkload = totalWorkload;
		int startIndex = 0;
		int count = 0;

		if (totalWorkload == 1)
		{
			cpus = 1;
		}

		std::deque<std::atomic<bool>> tickets;

#ifdef MULTITHREAD
		while (workload < 1 && cpus > 2)
		{
			cpus--;
			workload = totalWorkload / cpus;
		}

		if (workload < 1.f)
		{
			cpus = 0;
		}

		while (cpus > threads)
		{
			currentWorkload = floor(workload);
			float workloadFrac = fmod(workload, 1.0f);
			restWorkload = workloadFrac;

			tickets.emplace_back(false);
			pool->queueJob(evaluate, startIndex, currentWorkload + floor(restWorkload), std::ref(fitness), algo, std::ref(outputs), std::ref(inputs), 
				std::ref(networks), &tickets.back());
			++threads;

			count += currentWorkload + floor(restWorkload);

			startIndex += currentWorkload + floor(restWorkload);

			restWorkload -= floor(restWorkload);
			restWorkload += workloadFrac;
		}
#endif //MULTITHREAD
		currentWorkload = totalWorkload - count;

		evaluate(startIndex, currentWorkload, fitness, algo, outputs, inputs, networks);

		for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
		{
			itTicket->wait(false);
		}

		algo->setScore(fitness);

		algo->getGoat()->saveCurrentGenome();

		algo->evolve();

		for (int axis = 0; axis < 3; axis++)
		{
			algo->generateNetworks(networks[axis], inputSubstrate[axis], outputSubstrate, hiddenSubstrate);
		}
	}

	std::cout << "done" << std::endl;

	return false;
}


void evaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, Hyperneat* algo, const std::vector<glm::vec3>& outputs,
	const std::vector<std::vector<bool>>& inputs, std::vector<std::vector<NeuralNetwork>>& networks, std::atomic<bool>* ticket)
{
	for (int i = startIndex; i < (startIndex + currentWorkload); i++)
	{
		fitness[i] = sceneTest(networks, i, outputs, inputs, algo);
	}

	if (ticket != nullptr)
	{
		(*ticket) = true;
		ticket->notify_one();
	}
}

float sceneTest(std::vector<std::vector<NeuralNetwork>>& networks, int index, const std::vector<glm::vec3>& outputs,
	const std::vector<std::vector<bool>>& inputs, Hyperneat* algo)
{
	std::vector<float> networkOutputs;
	glm::vec3 normal;

	std::vector<float> inputsFloat;

	inputsFloat.resize(inputs[0].size());

	float score = outputs.size() * 2;

	for (int cpt = 0; cpt < outputs.size(); cpt++)
	{
		for (int i = 0; i < inputsFloat.size(); i++)
		{
			inputsFloat[i] = (inputs[cpt][i] == true ? 1 : 0);
		}

		for (int axis = 0; axis < 3; axis++)
		{
			networks[axis][index].compute(inputsFloat, networkOutputs);
			normal[axis] = networkOutputs[0];
		}

		//Do test
		normal = glm::normalize(normal);

		float square = 0;

		for (int axis = 0; axis < 3; axis++)
		{
			square += pow(normal[axis] - outputs[cpt][axis], 2);
		}

		score -= sqrt(square);
	}

	return score / 2000.0;
}