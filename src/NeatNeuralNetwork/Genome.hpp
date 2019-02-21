//
// Created by seb on 23/01/19.
//

#pragma once

#include <map>
#include <random>
#include <algorithm>
#include <memory>
#include "../SnakeAPI/snakeAPI.hpp"

struct MutationRate {
	MutationRate():
		connectionMutateChance(1.00),
		weightMutationChance(0.75),
		perturbChance(0.10),
		crossoverChance(0.75),
		linkMutationChance(2.0),
		nodeMutationChance(0.20),
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
		innovationNum(0),
		fromNode(0),
		toNode(0),
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

	Node(const Node &other) {
		this->id = other.id;
		this->value = other.value;
		this->calculated = other.calculated;
	}
	Node(unsigned int id) {
		this->id = id;
		this->value = 0.0;
		this->calculated = false;
	}

	double Update(std::map<unsigned int, std::unique_ptr<Connection>> &genes,
		      std::map<unsigned int, std::unique_ptr<Node>> &nodes,
		      NetworkInfo const &info) {
		if (id < info.inputSize + info.biasSize || calculated)
			return value;
		value = 0.0;
		for (const auto &gene : genes) {
			if (gene.second->toNode == id)
				value += nodes[gene.second->fromNode]->Update(genes, nodes, info);
		}
		value = value / (1 + std::abs(value));
		calculated = true;
		return value;
	}

	unsigned int	id;
	double		value;
	bool		calculated;
};

class Genome : public SnakeAPI {
public:
	Direction computeDirection() const final;

	Genome(NetworkInfo const &info, MutationRate const &rates);
	Genome(const Genome& other);
	Genome(Genome& pere, Genome& mere);

	~Genome() = default;


	Genome &operator=(const Genome &other);

	static unsigned int GetInnovation();
	static unsigned int GetNodeId();

	void Update();
	void Mutate();
	void Crossover(Genome &pere, Genome &mere);

	void WeightMutation();
	void ConnectionMutate();
	void ConnectionEnableMutation();
	void NodeMutation();

	MutationRate mutationRates;
	NetworkInfo networkInfo;
	std::map<unsigned int, std::unique_ptr<Connection>>	genes;
	std::map<unsigned int, std::unique_ptr<Node>>		nodes;

	unsigned int fitness = 0;
	unsigned int adjusted_fitness = 0;
	unsigned int global_rank = 0;
	unsigned int maxNeuron = 0;


	///RANDOM

	//std::mt19937 gen;
	//int gen;
};
