//
// Created by seb on 23/01/19.
//

#pragma once

#include <map>
#include <random>
#include <algorithm>
#include <memory>
#include "../SnakeAPI/snakeAPI.hpp"
#include "Node.hpp"

class Genome : public SnakeAPI {
public:
	Direction computeDirection() final;
	void graphicalTic() final;

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
	void Crossover(Genome &clone);

	void save(unsigned int generation);
	void load(std::string file);

	void GetInApple();
	void GetInBody();
	void GetInWall();

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
