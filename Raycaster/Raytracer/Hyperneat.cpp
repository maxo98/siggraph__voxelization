// Fill out your copyright notice in the Description page of Project Settings.


#include "Hyperneat.h"
#include "ThreadPool.h"

Hyperneat::Hyperneat(unsigned int _populationSize, const NeatParameters& _neatParam, const HyperneatParameters& _hyperParam, Neat::INIT init)
{
	hyperParam = _hyperParam;

	for (int i = 0; i < hyperParam.cppnInput.size(); i++)
	{
		cppns.push_back(new CPPN_Neat(_populationSize, hyperParam.cppnInput[i], hyperParam.cppnOutput[i], _neatParam, init));
	}
	
	networks.resize(_populationSize);
}

Hyperneat::Hyperneat(unsigned int _populationSize, const NeatParameters& _neatParam, const HyperneatParameters& _hyperParam, std::vector<Genome>& initPop)
{
	hyperParam = _hyperParam;

	for (int i = 0; i < hyperParam.cppnInput.size(); i++)
	{
		cppns.push_back(new CPPN_Neat(_populationSize, hyperParam.cppnInput[i], hyperParam.cppnOutput[i], _neatParam, initPop));
	}

	networks.resize(_populationSize);
}

Hyperneat::~Hyperneat()
{
	for (int i = 0; i < cppns.size(); i++)
	{
		delete cppns[i];
	}
}

void Hyperneat::addInput(const std::vector<float>& node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		std::cout << "Error adding input of wrong dimension" << std::endl;

		return;
	}

	inputSubstrate.push_back(node); 
}

void Hyperneat::addOutput(const std::vector<float>& node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		std::cout << "Error adding input of wrong dimension" << std::endl;

		return;
	}

	outputSubstrate.push_back(node);
}

void Hyperneat::addHiddenNode(unsigned int layer, const std::vector<float>& node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		std::cout << "Error adding input of wrong dimension\n";

		return;
	}

	hiddenSubstrates[layer].push_back(node);
};

void Hyperneat::clear()
{
	inputSubstrate.clear();
	outputSubstrate.clear();
	hiddenSubstrates.clear();
}

void Hyperneat::generateNetworks()
{
	generateNetworks(networks, inputSubstrate, outputSubstrate, hiddenSubstrates);
}

void Hyperneat::generateNetworks(std::vector<NeuralNetwork>& networks, std::vector<std::vector<float>>& inputSubstrate,
	std::vector<std::vector<float>>& outputSubstrate, std::vector<std::vector<std::vector<float>>>& hiddenSubstrates)
{
	networks.resize(cppns[0]->getPopSize());
	
	int threads = 1;
	ThreadPool* pool = ThreadPool::getInstance();
	size_t taskLaunched = pool->getTasksTotal();
	unsigned int cpus = (pool->getThreadPoolSize() >= taskLaunched ? pool->getThreadPoolSize() - taskLaunched : 0);

	float totalWorkload = cppns[0]->getPopSize();
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
		pool->queueJob(&Hyperneat::generateNetworksThread, this, startIndex, currentWorkload + floor(restWorkload), std::ref(networks),
			std::ref(inputSubstrate), std::ref(outputSubstrate), std::ref(hiddenSubstrates), &tickets.back());
		++threads;

		count += currentWorkload + floor(restWorkload);
		startIndex += currentWorkload + floor(restWorkload);

		restWorkload -= floor(restWorkload);
		restWorkload += workloadFrac;
	}
#endif // MULTITHREAD

	currentWorkload = totalWorkload - count;

	count += currentWorkload;

	generateNetworksThread(startIndex, currentWorkload, networks, inputSubstrate, outputSubstrate, hiddenSubstrates);

	for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
	{
		itTicket->wait(false);
	}
}

void Hyperneat::generateNetworksThread(int startIndex, int worlkload, std::vector<NeuralNetwork>& networks, std::vector<std::vector<float>>& inputSubstrate,
	std::vector<std::vector<float>>& outputSubstrate, std::vector<std::vector<std::vector<float>>>& hiddenSubstrates, std::atomic<bool>* ticket)
{
	std::vector<NeuralNetwork*> hyperNets;
	for (int i = 0; i < cppns.size(); i++)
	{
		hyperNets.push_back(nullptr);
	}

	for (unsigned int cpt = startIndex; cpt < (startIndex + worlkload); cpt++)
	{
		for (int i = 0; i < cppns.size(); i++)
		{
			hyperNets[i] = cppns[i]->getNeuralNetwork(cpt);
		}

		createNetwork(hyperNets, networks[cpt], inputSubstrate, outputSubstrate, hiddenSubstrates);
	}

	if (ticket != nullptr)
	{
		(*ticket) = true;
		ticket->notify_one();
	}
}

void Hyperneat::genomeToNetwork(std::vector<Genome*>& gen, NeuralNetwork& net)
{
	genomeToNetwork(gen, net, inputSubstrate, outputSubstrate, hiddenSubstrates);
}

void Hyperneat::genomeToNetwork(std::vector<Genome*>& gen, NeuralNetwork& net, std::vector<std::vector<float>>& inputSubstrate,
	std::vector<std::vector<float>>& outputSubstrate, std::vector<std::vector<std::vector<float>>>& hiddenSubstrates)
{
	std::vector<NeuralNetwork*> hyperNets;
	for (int i = 0; i < gen.size(); i++)
	{
		hyperNets.push_back(new NeuralNetwork());
		cppns[i]->genomeToNetwork(*gen[i], *hyperNets[i]);
	}
	initNetwork(net, inputSubstrate, outputSubstrate, hiddenSubstrates);
	createNetwork(hyperNets, net, inputSubstrate, outputSubstrate, hiddenSubstrates);

	for (int i = 0; i < gen.size(); i++)
	{
		delete hyperNets[i];
	}
}

void Hyperneat::initNetworks()
{
	initNetworks(networks, inputSubstrate, outputSubstrate, hiddenSubstrates);
}

void Hyperneat::initNetworks(std::vector<NeuralNetwork>& networks, std::vector<std::vector<float>>& inputSubstrate,
	std::vector<std::vector<float>>& outputSubstrate, std::vector<std::vector<std::vector<float>>>& hiddenSubstrates)
{
	for (int i = 0; i < networks.size(); i++)
	{
		initNetwork(networks[i], inputSubstrate, outputSubstrate, hiddenSubstrates);
	}
}

void Hyperneat::initNetwork(NeuralNetwork& net, std::vector<std::vector<float>>& inputSubstrate,
	std::vector<std::vector<float>>& outputSubstrate, std::vector<std::vector<std::vector<float>>>& hiddenSubstrates)
{
	//Add the input layer
	net.addMultipleInputNode(inputSubstrate.size());

	int layer = 1;
	for (std::vector<std::vector<std::vector<float>>>::iterator itLayer = hiddenSubstrates.begin(); itLayer != hiddenSubstrates.end(); ++itLayer, ++layer)
	{
		net.addHiddenNode(itLayer->size(), layer, hyperParam.activationFunction);
	}

	//Add and connect the output layer
	net.addOutputNode(outputSubstrate.size(), hyperParam.activationFunction);
}

void Hyperneat::createNetwork(int cppnIndex, NeuralNetwork& net, std::vector<std::vector<float>>& inputSubstrate,
	std::vector<std::vector<float>>& outputSubstrate, std::vector<std::vector<std::vector<float>>>& hiddenSubstrates)
{
	std::vector<NeuralNetwork*> hyperNets;
	for (int i = 0; i < cppns.size(); i++)
	{
		hyperNets[i] = cppns[i]->getNeuralNetwork(cppnIndex);
	}
	createNetwork(hyperNets, net, inputSubstrate, outputSubstrate, hiddenSubstrates);
}

void Hyperneat::createNetwork(std::vector<NeuralNetwork*>& hypernets, NeuralNetwork& net, std::vector<std::vector<float>>& inputSubstrate,
	std::vector<std::vector<float>>& outputSubstrate, std::vector<std::vector<std::vector<float>>>& hiddenSubstrates)
{
	net.clearConnections();

	std::vector<std::vector<float>>::iterator beginPreviousLayer = inputSubstrate.begin();
	std::vector<std::vector<float>>::iterator endPreviousLayer = inputSubstrate.end();

	int layer = 1;

	std::vector<NeuralNetwork*>::iterator hyperNet = hypernets.begin();
	int cppnIndex = 0;

	//Connect the hidden layers
	for (std::vector<std::vector<std::vector<float>>>::iterator itLayer = hiddenSubstrates.begin(); itLayer != hiddenSubstrates.end(); ++itLayer)
	{
		connectLayer(layer, **hyperNet, net, itLayer->begin(), itLayer->end(), beginPreviousLayer, endPreviousLayer, cppnIndex);

		beginPreviousLayer = itLayer->begin();
		endPreviousLayer = itLayer->end();

		if (hypernets.size() > 2)
		{
			++hyperNet;
			++cppnIndex;
		}

		layer++;
	}

	++hyperNet;
	++cppnIndex;

	connectLayer(layer, **hyperNet, net, outputSubstrate.begin(), outputSubstrate.end(), beginPreviousLayer, endPreviousLayer, cppnIndex);
}

/**
* Connect layer to the previous layer
*/
void Hyperneat::connectLayer(unsigned int layer, NeuralNetwork& hypernet, NeuralNetwork& net, std::vector<std::vector<float>>::iterator itNode,
	std::vector<std::vector<float>>::iterator itNodeEnd,
	std::vector<std::vector<float>>::iterator beginPreviousLayer, const std::vector<std::vector<float>>::iterator endPreviousLayer, int cppnIndex)
{
	int nodeB = 0;

	//For each node in the layer to add
	for (itNode; itNode != itNodeEnd; ++itNode)
	{
		std::vector<std::vector<float>>::iterator prevLayer = beginPreviousLayer;

		int nodeA = 0;
		std::vector<float> p2 = std::vector<float>(itNode->begin(), itNode->end());
		

		//For each node from the previous layer
		for (prevLayer; prevLayer != endPreviousLayer; ++prevLayer)
		{
			std::vector<float> output, input;
			std::vector<float> p1 = std::vector<float>(prevLayer->begin(), prevLayer->end());
			input = hyperParam.cppnInputFunction[cppnIndex](hyperParam.inputVariables, p1, p2);

			hypernet.compute(input, output);

			//Check if we should create a connection
			if (hyperParam.thresholdFunction[cppnIndex](hyperParam.thresholdVariables, output, p1, p2) == true)
			{
				float weight = hyperParam.weightModifierFunction[cppnIndex](hyperParam.weightVariables, output, p1, p2);

				net.connectNodes(layer - 1, nodeA, layer, nodeB, weight);
			}

			nodeA++;
		}

		nodeB++;
	}
}


//Only compute backprop over connections of output nodes
bool Hyperneat::backprop(const std::vector<float>& inputs, const std::vector<float>& outputs, float learnRate)
{
	if (inputs.size() < inputSubstrate.size() || outputs.size() < outputSubstrate.size()) return false;

	generateNetworks();

	for (int i = 0; i < networks.size(); i++)
	{
		std::vector<float> errors;

		int cpt = 0;

		networks[i].compute(inputs, errors);

		std::vector<float> cppnInputs, cppnOutputs;
		cppnOutputs.push_back(0);

		//Compute the error for each connection entering an output nodes
		//And correct it one by one
		for (std::deque<Node>::iterator it = networks[i].getOutputNodes()->begin(); it != networks[i].getOutputNodes()->end(); ++it, ++cpt)
		{
			
			float delta = (it->getValue() - outputs[i]) * it->getActivation()->derivate(it->getBackpropValue(), it->getValue());

			if (delta == 0) continue;
			
			for (int i2 = 0; i2 < it->getPreviousNodes()->size(); ++i2)
			{
				std::vector<float>* p1;

				if (hiddenSubstrates.size() == 0)
				{
					p1 = &inputSubstrate[i2];
				}
				else {
					p1 = &hiddenSubstrates.back()[i2];
				}

				cppnInputs = hyperParam.cppnInputFunction[0](hyperParam.inputVariables, *p1, outputSubstrate[cpt]);

				if (hyperParam.cppnOutput[0] > 1)
				{
					cppns[0]->getNeuralNetwork(i)->compute(cppnInputs, cppnOutputs);
				}

				cppnOutputs[0] = (*it->getPreviousNodes())[i2].second - learnRate * delta * (*it->getPreviousNodes())[i2].first->getValue();
				cppnOutputs[0] = hyperParam.inverseWeightModifierFunction(hyperParam.weightVariables, cppnOutputs, *p1, outputSubstrate[cpt]);
				
				cppns[0]->getNeuralNetwork(i)->backprop(cppnInputs, cppnOutputs, learnRate);
			}
		}

		std::vector<NeuralNetwork*> hyperNet;
		hyperNet.push_back(cppns[0]->getNeuralNetwork(i));

		createNetwork(hyperNet, networks[i], inputSubstrate, outputSubstrate, hiddenSubstrates);
	}

	return true;
}

void Hyperneat::applyBackprop()
{
	for (int i = 0; i < networks.size(); i++)
	{
		networks[i].applyBackprop(cppns[0]->getGenomes()[i]);
	}
}

std::vector<float> basicCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p2.begin(), p2.end());
	return p1;
}

std::vector<float> biasCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p2.begin(), p2.end());
	p1.push_back(1.0f);
	return p1;
}

std::vector<float> sqrDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	float dist = 0;

	for (int i = 0; i < p1.size(); i++)
	{
		
		dist += (p2[i] - p1[i]) * (p2[i] - p1[i]);
	}

	p1.insert(p1.end(), p2.begin(), p2.end());
	p1.push_back(dist);

	return p1;
}

std::vector<float> invDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	float dist = 0;

	for (int i = 0; i < p1.size(); i++)
	{
		dist += (p2[i] - p1[i]) * (p2[i] - p1[i]);
	}

	p1.insert(p1.end(), p2.begin(), p2.end());

	dist = *((float *)variables[0]) - sqrt(dist);

	p1.push_back(dist);

	return p1;
}

std::vector<float> deltaDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p1.begin(), p1.end());

	for (int i = 0; i < p2.size(); i++)
	{
		p1.push_back((p2[i] - p1[i]));
	}

	return p1;
}