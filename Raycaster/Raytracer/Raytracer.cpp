#include "Window.h"

#include <SDL.h>
#include <gtx/quaternion.hpp>
#include "VoxelScene.h"
#include "ThreadPool.h"
#include <iomanip>
#include "Genome.h"
#include "Neat.h"
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PROGRAM_NAME "Raycaster V1.0"

#define windowWidth 1920
#define windowHeight 1080

#define MULTITHREAD

#define RADIUS 5

#define OCTSIZE 0.00390625

#define GEN 50

struct PosNodeHash {
public:
	size_t operator()(const glm::ivec4& node) const
	{
		std::size_t value = node[0];

		for (int i = 1; i < 4; i++)
		{
			std::size_t tmp = 0;

			value = value ^ (node[i] << i);
		}

		return value; // or use boost::hash_combine
	}
};

float sceneTest(NeuralNetwork& network, const std::vector<std::vector<float>>& outputs,
	const std::vector<std::vector<bool>>& inputs);

void addLayer(int& id, int radius, int layer, std::unordered_map<glm::ivec4, unsigned int, PosNodeHash>& nodesId, Activation* activation, std::vector<GeneNode>* nodes);

void connectLayer(int radius, int layer, std::unordered_map<glm::ivec4, unsigned int, PosNodeHash>& nodesId, Genome& gen,
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, long long seed);

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
	Activation* relu = new ReluActivation();
	Activation* linear = new LinearActivation();
	arrActiv.push_back(relu);

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

	std::unordered_map<glm::ivec4, unsigned int, PosNodeHash> nodesId;
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> allConnections;

	std::vector<GeneNode>* nodes = gen.getNodes();

	nodes->push_back(GeneNode(NODE_TYPE::INPUT, relu));

	int id = 1;
	int layer = 0;
	int input = RADIUS * RADIUS * RADIUS;
	int output = 3;

	for (int rad = RADIUS; rad > 0; rad--)
	{
		addLayer(id, rad, layer, nodesId, relu, nodes);
		layer++;
	}

	nodes->push_back(GeneNode(NODE_TYPE::HIDDEN, linear, layer));
	nodes->push_back(GeneNode(NODE_TYPE::HIDDEN, linear, layer));
	nodes->push_back(GeneNode(NODE_TYPE::HIDDEN, linear, layer));


	layer = 1;

	for (int rad = RADIUS-1; rad > 0; rad--)
	{
		connectLayer(rad, layer, nodesId, gen, allConnections, seed);

		layer++;
	}

	glm::ivec4 nodePos;

	nodePos.w = layer - 1;

	for (int x2 = -1; x2 <= 1; x2++)
	{
		nodePos.x = x2;

		for (int y2 = -1; y2 <= 1; y2++)
		{
			nodePos.y = y2;

			for (int z2 = -1; z2 <= 1; z2++)
			{
				nodePos.z = z2;

				for (int i = 0; i < 3; i++)
				{
					gen.addConnection(nodesId[nodePos], id + i, allConnections, lecunUniformInit(9, 3, seed));
				}
			}
		}
	}

	Neat::genomeToNetwork(gen, network);

	//Do test
	int epoch = 100;
	float lRate = 0.0001;

	unsigned int percent = 0;
	unsigned int div = epoch / 100.f;

	if (div == 0) div = 1;

	std::cout << "Training" << std::endl;

	std::cout << "Progress: " << percent << "%" << std::endl;

	std::vector<float> inputsFloat;
	inputsFloat.resize(inputs[0].size()+1);

	inputsFloat.back() = 0.5;

	std::vector<unsigned int> indexList;

	for (int i = 0; i < inputs.size(); i++)
	{
		indexList.push_back(i);
	}

	auto randomEngine = std::default_random_engine(seed);

	for (int i = 0; i < epoch; i++)
	{
		shuffle(indexList.begin(), indexList.end(), randomEngine);


		for (std::vector<unsigned int>::iterator itIndex = indexList.begin(); itIndex != indexList.end(); ++itIndex)
		{
			for (int i = 0; i < inputsFloat.size(); i++)
			{
				inputsFloat[i] = (inputs[*itIndex][i] == true ? 1 : 0);
			}

			network.backprop(inputsFloat, outputs[*itIndex], lRate, true);
		}


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
	delete relu;
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


void addLayer(int& id, int radius, int layer, std::unordered_map<glm::ivec4, unsigned int, PosNodeHash>& nodesId, Activation* activation, std::vector<GeneNode>* nodes)
{
	glm::ivec4 pos;

	pos.w = layer;

	for (int x = 0; x < radius; x++)
	{
		pos.x = x;

		for (int y = 0; y < radius; y++)
		{
			pos.y = y;

			for (int z = 0; z < radius; z++)
			{
				pos.z = z;

				nodesId.emplace(pos, id);

				if (layer == 0)
				{
					nodes->push_back(GeneNode(NODE_TYPE::INPUT, activation));
				}
				else {
					nodes->push_back(GeneNode(NODE_TYPE::HIDDEN, activation, layer));
				}

				id++;
			}
		}
	}
}


void connectLayer(int radius, int layer, std::unordered_map<glm::ivec4, unsigned int, PosNodeHash>& nodesId, Genome& gen,
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, long long seed)
{

	glm::ivec4 pos1, pos2;

	pos2.w = layer;
	pos1.w = layer - 1;

	for (int x = 0; x < radius; x++)
	{
		pos1.x = x;

		for (int y = 0; y < radius; y++)
		{
			pos1.y = y;

			for (int z = 0; z < radius; z++)
			{
				pos1.z = z;


				for (int x2 = -1; x2 <= 1; x2++)
				{
					pos2.x = x + x2;

					for (int y2 = -1; y2 <= 1; y2++)
					{
						pos2.y = y + y2;

						for (int z2 = -1; z2 <= 1; z2++)
						{
							pos2.z = z + z2;

							gen.addConnection(nodesId[pos1], nodesId[pos2], allConnections, heUniformInit(pow(radius + 1, 3), pow(radius, 3), seed));
						}
					}
				}

				gen.addConnection(0, nodesId[pos2], allConnections, 0);
			}
		}
	}
}