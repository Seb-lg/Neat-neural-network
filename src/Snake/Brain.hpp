//
// Created by seb on 06/02/19.
//

#ifndef NEAT_NEURAL_NETWORK_BRAIN_HPP
#define NEAT_NEURAL_NETWORK_BRAIN_HPP


#include <Genome.hpp>

class Brain {
public:
	Brain(): info(), rates(), genome(info, rates) {}
private:
	NetworkInfo info;
	MutationRate rates;
	Genome genome;
};


#endif //NEAT_NEURAL_NETWORK_BRAIN_HPP
