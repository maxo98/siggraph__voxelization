#include "Window.h"

#include <SDL.h>
#include <gtx/quaternion.hpp>
#include "VoxelScene.h"
#include "ThreadPool.h"
#include <iomanip>
#include "Hyperneat.h"


#define PROGRAM_NAME "Raycaster V1.0"

#define windowWidth 1920
#define windowHeight 1080

#define MULTITHREAD

bool hypeneatTest(int popSize, std::vector<VoxelScene*>& scenes, Hyperneat& algo);
void evaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, Hyperneat& esHyper, bool& validated, std::vector<VoxelScene*>& scenes, std::atomic<bool>* ticket = nullptr);
int sceneTest(NeuralNetwork* network, bool display, bool& validated, std::vector<VoxelScene*>& scenes);

#define LOAD

std::vector<float> upscaleCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	float dist = 0;

	for (int i = 0; i < p1.size(); i++)
	{

		dist += (p2[i] - p1[i]) * (p2[i] - p1[i]);
	}

	std::vector<float> p3;

	p3.push_back(2 - sqrt(dist));

	p3.push_back(1);//Bias

	return p3;
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

	Window window("Raycaster V1.0", windowWidth, windowHeight);

	int renderScene = 0;// 4;
	std::vector<VoxelScene*> scenes;
	scenes.reserve(8);

	scenes.push_back(new VoxelScene(7));
	scenes.push_back(new VoxelScene(8));
	scenes.push_back(new VoxelScene(7));
	scenes.push_back(new VoxelScene(8));
	scenes.push_back(new VoxelScene(7));
	scenes.push_back(new VoxelScene(8));
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

	scenes[0]->loadModel(glm::dvec3(3, 2.01, 3), "Spaceship7.txt");
	scenes[1]->loadModel(glm::dvec3(3, 2.01, 3), "Spaceship8.txt");

	scenes[2]->loadModel(glm::dvec3(3, 2.01, 3), "Cannon7.txt");
	scenes[3]->loadModel(glm::dvec3(3, 2.01, 3), "Cannon8.txt");

	scenes[4]->loadModel(glm::dvec3(3, 2.01, 3), "doughnut7.txt");
	scenes[5]->loadModel(glm::dvec3(3, 2.01, 3), "doughnut8.txt");

	//scenes[6]->loadModel(glm::dvec3(3, 2.01, 3), "Cube7.txt");
	//scenes[7]->loadModel(glm::dvec3(3, 2.01, 3), "Cube8.txt");

	std::cout << "Simplifying\n";

	for (int i = 0; i < scenes.size(); i++)
	{
		scenes[i]->simplify();
	}

	std::cout << "Done\n";



	NeatParameters neatparam;

	//neatparam.activationFunctions.push_back(new thresholdActivation());
	neatparam.activationFunctions.push_back(new SigmoidActivation());
	neatparam.activationFunctions.push_back(new SinActivation());
	neatparam.activationFunctions.push_back(new GaussianActivation());
	neatparam.activationFunctions.push_back(new AbsActivation());

	neatparam.pbMutateLink = 0.05;// 0.05;
	neatparam.pbMutateNode = 0.03;//0.03;
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

	neatparam.speciationDistance = 2.0;


	neatparam.speciationDistanceMod = 0.3;
	neatparam.minExpectedSpecies = 6;
	neatparam.maxExpectedSpecies = 12;
	neatparam.adaptSpeciation = true;

	neatparam.keepChamp = true;
	neatparam.elistism = true;
	neatparam.rouletteMultiplier = 2.0;

	HyperneatParameters hyperneatParam;

	hyperneatParam.activationFunction = new LinearActivation();
	hyperneatParam.cppnInput = 2;
	hyperneatParam.cppnInputFunction = upscaleCppnInput;
	hyperneatParam.cppnOutput = 1;
	hyperneatParam.nDimensions = 3;
	hyperneatParam.thresholdFunction = noThreshold;
	hyperneatParam.weightModifierFunction = absWeight;

	int popSize = 150;

	int result = 0;

	std::vector<float> inputPos;
	inputPos.resize(3);
	std::vector<float> outputPos;
	outputPos.resize(3);

	int count = 0;

	Hyperneat hyper(popSize, neatparam, hyperneatParam, Neat::INIT::FULL);

	//Set node location
	//Have 8 output for the 8 possibles voxel positions
	//Feed in inverse distance between 0 and 1

	//input - 1, -1, -1
	//input - 1, -1, 0
	//input - 1, -1, 1
	//input - 1, 0, -1
	//input - 1, 0, 0
	//input - 1, 0, 1
	//input - 1, 1, -1
	//input - 1, 1, 0
	//input - 1, 1, 1
	//input 0, -1, -1
	//input 0, -1, 0
	//input 0, -1, 1
	//input 0, 0, -1
	//input 0, 0, 0
	//input 0, 0, 1
	//input 0, 1, -1
	//input 0, 1, 0
	//input 0, 1, 1
	//input 1, -1, -1
	//input 1, -1, 0
	//input 1, -1, 1
	//input 1, 0, -1
	//input 1, 0, 0
	//input 1, 0, 1
	//input 1, 1, -1
	//input 1, 1, 0
	//input 1, 1, 1

	//output 0, 0, 0
	//output 0, 0, 0.5
	//output 0, 0.5, 0
	//output 0, 0.5, 0.5
	//output 0.5, 0, 0
	//output 0.5, 0, 0.5
	//output 0.5, 0.5, 0
	//output 0.5, 0.5, 0.5

	for (int x = -1; x <= 1; x++)
	{
		inputPos[0] = x;
		outputPos[0] = x / 2.0f;

		for (int y = -1; y <= 1; y++)
		{
			inputPos[1] = y;
			outputPos[1] = y / 2.0f;

			for (int z = -1; z <= 1; z++)
			{
				inputPos[2] = z;
				outputPos[2] = z / 2.0f;

				if (x != 0 || y != 0 || z != 0)
				{
					hyper.addInput(inputPos);
				}

				if (x >= 0 && y >= 0 && z >= 0)
				{
					hyper.addOutput(outputPos);
				}
			}
		}
	}

#ifndef LOAD
	hyper.initNetworks();

	hyper.generateNetworks();

	//Do test
	//Also test for single isolated voxel
	hypeneatTest(popSize, scenes, hyper);

	//Save
	hyper.saveHistory();

	hyper.getGoat()->saveCurrentGenome();
#endif // !LOAD

	//Apply to check result

	NeuralNetwork network;

#ifndef LOAD
	hyper.genomeToNetwork(*hyper.getGoat(), network);
#else
	Genome gen = Genome::loadGenome("saveGenome.txt");

	hyper.genomeToNetwork(gen, network);
#endif // !LOAD

	std::vector<float> inputs;
	inputs.resize(3 * 3 * 3 - 1);
	std::vector<float> expectedOutputs;
	expectedOutputs.resize(8);
	std::vector<float> networkOutputs;

	const double high = 0.00390625;
	const double low = 0.0078125;

	glm::dvec3 inputPosition, outputPosition;
	glm::vec3 colorHolder;

	glm::dvec3 voxPos(0);

	scenes.push_back(new VoxelScene(8));


	for (voxPos[0] = scenes[renderScene]->min[0] - low; voxPos[0] <= (scenes[renderScene]->max[0] + low); voxPos[0] += low)
	{
		for (voxPos[1] = scenes[renderScene]->min[1] - low; voxPos[1] <= (scenes[renderScene]->max[1] + low); voxPos[1] += low)
		{
			for (voxPos[2] = scenes[renderScene]->min[2] - low; voxPos[2] <= (scenes[renderScene]->max[2] + low); voxPos[2] += low)
			{
				int inputCpt = 0;
				int outputCpt = 0;

				for (int x = -1; x <= 1; x++)
				{
					inputPosition[0] = x * low + voxPos[0];
					outputPosition[0] = x * high + voxPos[0];

					for (int y = -1; y <= 1; y++)
					{
						inputPosition[1] = y * low + voxPos[1];
						outputPosition[1] = y * high + voxPos[1];

						for (int z = -1; z <= 1; z++)
						{
							inputPosition[2] = z * low + voxPos[2];
							outputPosition[2] = z * high + voxPos[2];

							inputs[inputCpt] = (scenes[renderScene]->readPoint(inputPosition, colorHolder, 7) ? 1 : 0);
							inputCpt++;

							if (x >= 0 && y >= 0 && z >= 0)
							{
								//expectedOutputs[outputCpt] = (scenes[renderScene + 1]->readPoint(outputPosition, colorHolder, 8) ? 1 : 0);
								outputCpt++;
							}
						}
					}
				}

				network.compute(inputs, networkOutputs);

				int i = 0;
				for (int x = 0; x <= 1; x++)
				{
					outputPosition[0] = x * high + voxPos[0];

					for (int y = 0; y <= 1; y++)
					{
						outputPosition[1] = y * high + voxPos[1];

						for (int z = 0; z <= 1; z++)
						{
							outputPosition[2] = z * high + voxPos[2];

							if (networkOutputs[i] >= 1)
							{
								colorHolder = glm::vec3(0.8, 0.8, 0.8);
								scenes[scenes.size() - 1]->addPoint(outputPosition, colorHolder);
							}

							i++;
						}
					}
				}
			}
		}
	}

	scenes[scenes.size() - 1]->simplify();
	scenes[scenes.size() - 1]->addPointLight(glm::vec3(3.5, 4, 0));

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

	//Camera variables
	glm::vec3 pos(3, 3.25, 0);  //x and y start position
	float deg = 45;
	glm::quat camRot;

	Camera cam(pos, camRot, 60, windowWidth, windowHeight);
	cam.lookAt(glm::vec3(3, 3.5, 3.3));
	
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
					pool->queueJob(&VoxelScene::drawPixels, scenes[scenes.size() - 1], currentWorkload, x, y, std::ref(window), std::ref(cam), std::ref(buffer), &tickets.back());
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

				scenes[scenes.size() - 1]->drawPixels(currentWorkload, x, y, window, cam, buffer);

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

bool hypeneatTest(int popSize, std::vector<VoxelScene*>& scenes, Hyperneat& algo)
{
	std::vector<float> fitness;

	fitness.resize(popSize);

	bool validated = false;

	for (int i3 = 0; i3 < 1 && validated == false; i3++)
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
			pool->queueJob(evaluate, startIndex, currentWorkload + floor(restWorkload), std::ref(fitness), std::ref(algo), std::ref(validated), std::ref(scenes), &tickets.back());
			++threads;

			count += currentWorkload + floor(restWorkload);

			startIndex += currentWorkload + floor(restWorkload);

			restWorkload -= floor(restWorkload);
			restWorkload += workloadFrac;
		}
#endif //MULTITHREAD
		currentWorkload = totalWorkload - count;

		evaluate(startIndex, currentWorkload, fitness, algo, validated, scenes);

		for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
		{
			itTicket->wait(false);
		}

		algo.setScore(fitness);

		algo.evolve();
	}

	std::cout << "done" << std::endl;

	return false;
}


void evaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, Hyperneat& algo, bool& validated, std::vector<VoxelScene*>& scenes, std::atomic<bool>* ticket)
{
	for (int i = startIndex; i < (startIndex + currentWorkload); i++)
	{
		bool tmp = false;

		fitness[i] = sceneTest(algo.getNeuralNetwork(i), false, tmp, scenes);

		if (tmp == true)
		{
			validated = true;
		}
	}

	if (ticket != nullptr)
	{
		(*ticket) = true;
		ticket->notify_one();
	}
}

int sceneTest(NeuralNetwork* network, bool display, bool& validated, std::vector<VoxelScene*>& scenes)
{
	int good = 0;
	int error = 0;

	std::vector<float> inputs;
	inputs.resize(3 * 3 * 3 - 1);
	std::vector<float> expectedOutputs;
	expectedOutputs.resize(8);
	std::vector<float> networkOutputs;

	const double high = 0.00390625;
	const double low = 0.0078125;

	glm::dvec3 inputPos, outputPos; 
	glm::vec3 colorHolder;

	glm::dvec3 pos(0);

	for (int sceneIndex = 0; sceneIndex < scenes.size(); sceneIndex+=2)
	{
		for (pos[0] = scenes[sceneIndex]->min[0] - low; pos[0] <= (scenes[sceneIndex]->max[0] + low); pos[0] += low)
		{
			for (pos[1] = scenes[sceneIndex]->min[1] - low; pos[1] <= (scenes[sceneIndex]->max[1] + low); pos[1] += low)
			{
				for (pos[2] = scenes[sceneIndex]->min[2] - low; pos[2] <= (scenes[sceneIndex]->max[2] + low); pos[2] += low)
				{
					int inputCpt = 0;
					int outputCpt = 0;

					for (int x = -1; x <= 1; x++)
					{
						inputPos[0] = x * low + pos[0];
						outputPos[0] = x * high + pos[0];

						for (int y = -1; y <= 1; y++)
						{
							inputPos[1] = y * low + pos[1];
							outputPos[1] = y * high + pos[1];

							for (int z = -1; z <= 1; z++)
							{
								inputPos[2] = z * low + pos[2];
								outputPos[2] = z * high + pos[2];

								if (x != 0 || y != 0 || z != 0)
								{
									inputs[inputCpt] = (scenes[sceneIndex]->readPoint(inputPos, colorHolder, 7) ? 1 : 0);
									inputCpt++;
								}

								if (x >= 0 && y >= 0 && z >= 0)
								{
									expectedOutputs[outputCpt] = (scenes[sceneIndex + 1]->readPoint(outputPos, colorHolder, 8) ? 1 : 0);
									outputCpt++;
								}
							}
						}
					}

					network->compute(inputs, networkOutputs);

					for (int i = 0; i < 8; i++)
					{
						if ((networkOutputs[i] >= 1 && expectedOutputs[i] == 1) || (networkOutputs[i] < 0 && expectedOutputs[i] == 0))
						{
							good += 1;
						}
						else {
							error++;							
						}
					}
				}
			}
		}

		//Single isolated voxel test
		//for (int i = 0; i < inputs.size(); i++)
		//{
		//	inputs[i] = 0;
		//}

		//inputs[13] = 0;

		//network->compute(inputs, networkOutputs);

		//for (int i = 0; i < 8; i++)
		//{
		//	if (networkOutputs[i] >= 1 && expectedOutputs[i] == 1)
		//	{
		//		good++;
		//	}
		//	else {
		//		error++;
		//	}
		//}
	}

	if (error == 0)
	{
		validated = true;
	}

	return good;
}