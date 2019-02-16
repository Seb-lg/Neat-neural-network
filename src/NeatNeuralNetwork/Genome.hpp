//
// Created by seb on 23/01/19.
//

#pragma once

#include <map>
#include <random>
#include <algorithm>

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

struct Connection {
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

class Node {
public:
	Node() {
		this->id = 0;
		this->value = 0.0;
		this->calculated = false;
	}
	Node(unsigned int const &id) {
		this->id = id;
		this->value = 0.0;
		this->calculated = false;
	}

	double Update(std::map<unsigned int, Connection> &genes, std::map<unsigned int, Node> &nodes, NetworkInfo const &info) {
		if (id < info.inputSize + info.biasSize || calculated)
			return value;
		value = 0.0;
		for (const auto &gene : genes) {
			if (gene.second.toNode == id)
				value += nodes[gene.second.fromNode].Update(genes, nodes, info);
		}
		value = value / (1 + std::abs(value));
		calculated = true;
		return value;
	}

	unsigned int	id;
	double		value;
	bool		calculated;
};

class Genome {
public:

	Genome(NetworkInfo& info, MutationRate& rates);
	Genome(const Genome& other);
	Genome(Genome& pere, Genome& mere);

	static unsigned int GetInnovation();

	void Update();
	void Mutate();
	void Crossover(Genome &pere, Genome &mere);

	void WeightMutation();
	void ConnectionMutate();
	void ConnectionEnableMutation();
	void NodeMutation();

	MutationRate mutationRates;
	NetworkInfo networkInfo;
	std::map<unsigned int, Connection>	genes;
	std::map<unsigned int, Node>		nodes;

	unsigned int fitness = 0;
	unsigned int adjusted_fitness = 0;
	unsigned int global_rank = 0;
	unsigned int maxNeuron = 0;


	///RANDOM
	std::random_device rd;
	std::mt19937 gen;
};
