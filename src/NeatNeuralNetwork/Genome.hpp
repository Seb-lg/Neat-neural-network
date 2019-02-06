//
// Created by seb on 23/01/19.
//

#pragma once

#include <map>
#include <random>

struct MutationRate {
	MutationRate():
		connectionMutateChance(0.25),
		weightMutationChance(0.75),
		perturbChance(0.10),
		crossoverChance(0.75),
		linkMutationChance(2.0),
		nodeMutationChance(0.50),
		biasMutationChance(0.40),
		stepSize(0.10),
		disableMutationChance(0.4),
		enableMutationChance(0.2){
	}
	double connectionMutateChance;
	double weightMutationChance;
	double perturbChance;
	double crossoverChance;
	double linkMutationChance;
	double nodeMutationChance;
	double biasMutationChance;
	double stepSize;
	double disableMutationChance;
	double enableMutationChance;
};

struct SpeciatingParameter {
	SpeciatingParameter():
		population(240),
		deltaDisjoint(2.0),
		deltaWeights(0.4),
		deltaThreshold(1.3),
		staleSpecies(15) {

	}
	unsigned int	population;
	double		deltaDisjoint;
	double		deltaWeights;
	double		deltaThreshold;
	unsigned int	staleSpecies;
};

struct NetworkInfo {
	unsigned int	inputSize;
	unsigned int	biasSize;
	unsigned int	outputSize;
	unsigned int	functionalNodes;
};

struct Connection{
	Connection():
		innovationNum(-1),
		fromNode(-1),
		toNode(-1),
		weight(0.0),
		enabled(true){
	}
	unsigned int	innovationNum;
	unsigned int	fromNode;
	unsigned int	toNode;
	double		weight;
	bool		enabled;
};

class Genome {
public:


	Genome(NetworkInfo& info, MutationRate& rates):gen(rd()){
		mutationRates = rates;
		networkInfo = info;
		maxNeuron = networkInfo.functionalNodes;

		for (int i = 0; i < networkInfo.inputSize + 1 + networkInfo.outputSize; i++){
			genes[i];
		}
	}

	Genome(const Genome& other):gen(rd()) {
		this->mutationRates = other.mutationRates;
		this->networkInfo = other.networkInfo;
		this->genes = other.genes;
		this->maxNeuron = other.maxNeuron;
	}

	void mutate();

	void WeightMutation();
	void ConnectionMutate();
	void ConnectionEnableMutation();
	void NodeMutation();

	MutationRate mutationRates;
	NetworkInfo networkInfo;
	std::map<unsigned int, Connection> genes;

	unsigned int fitness = 0;
	unsigned int adjusted_fitness = 0;
	unsigned int global_rank = 0;
	unsigned int maxNeuron = 0;
	unsigned int can_be_recurrent = false;


	///RANDOM
	std::random_device rd;
	std::mt19937 gen;
};
