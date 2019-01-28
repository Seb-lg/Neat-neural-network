//
// Created by seb on 23/01/19.
//

#pragma once

#include <map>

typedef struct MutationRate {
	MutationRate():
		connectionMutateChance(0.25),
		perturbChance(0.90),
		crossoverChance(0.75),
		linkMutationChance(2.0),
		nodeMutationChance(0.50),
		biasMutationChance(0.40),
		stepSize(0.1),
		disableMutationChance(0.4),
		enableMutationChance(0.2){
	}
	double connectionMutateChance;
	double perturbChance;
	double crossoverChance;
	double linkMutationChance;
	double nodeMutationChance;
	double biasMutationChance;
	double stepSize;
	double disableMutationChance;
	double enableMutationChance;
};

typedef struct SpeciatingParameter {
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

typedef struct NetworkInfo {
	unsigned int	inputSize;
	unsigned int	biasSize;
	unsigned int	outputSize;
	unsigned int	functionalNodes;
	bool		recurrent;
};

typedef struct Connection{
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

	MutationRate mutationRates;
	NetworkInfo networkInfo;

	std::map<unsigned int, Connection> genes;

	Genome(NetworkInfo& info, MutationRate& rates){
		mutationRates = rates;
		networkInfo = info;
		maxNeuron = networkInfo.functionalNodes;
	}

	Genome(const Genome&) = default;

	unsigned int fitness = 0;
	unsigned int adjusted_fitness = 0;
	unsigned int global_rank = 0;
	unsigned int maxNeuron;
	unsigned int can_be_recurrent = false;
};
