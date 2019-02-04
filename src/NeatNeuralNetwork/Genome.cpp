//
// Created by seb on 23/01/19.
//

#include "Genome.hpp"
#include <iostream>

void Genome::mutate() {
	WeightMutation();
	ConnectionMutate();
	LinkMutation();
	NodeMutation();
	BiasMutation();
}

void Genome::WeightMutation() {
	std::uniform_real_distribution<double> rand(0.0, 1.0);
	std::uniform_real_distribution<double> weight(-1.0, 1.0);
	std::uniform_real_distribution<double> step(-(mutationRates.stepSize/2), (mutationRates.stepSize/2));

	for (auto &gene : genes) {
		if (rand(gen) < mutationRates.weightMutationChance) {
			if (rand(gen) < mutationRates.perturbChance){
				gene.second.weight = weight(gen);
			} else {
				gene.second.weight += step(gen); 
			}

		}
	}
}

void Genome::ConnectionMutate() {
	std::uniform_int_distribution<int> rand(0, genes.size());
	bool isOk = false;
	int from;
	int to;

	while (!isOk) {
		from = rand(gen);
		to = rand(gen);
		
	}
}