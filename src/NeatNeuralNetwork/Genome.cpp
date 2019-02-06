//
// Created by seb on 23/01/19.
//

#include "Genome.hpp"
#include <iostream>
#include <queue>

void Genome::mutate() {
	WeightMutation();
	ConnectionMutate();
	ConnectionEnableMutation();
	NodeMutation();
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
	std::uniform_int_distribution<int> rand(0, maxNeuron);
	std::uniform_real_distribution<double> weight(-1.0, 1.0);
	bool isOk = false;
	int from;
	int to;

	std::vector<std::vector<unsigned int>>	connections;
	connections.resize(maxNeuron);
	for (const auto &gene : genes)
		connections[gene.second.fromNode].push_back(gene.second.toNode);

	std::queue<unsigned int> qNodes;

	while (!isOk) {
		from = rand(gen);
		to = rand(gen);

		for (unsigned int item = 0; item < connections[to].size(); item++) {
			qNodes.push(connections[to][item]);
			while (!qNodes.empty()) {
				unsigned int tmp = qNodes.front();
				if (tmp == from)
					isOk = false;
				for (const auto &newN : connections[tmp])
					qNodes.push(newN);
			}

		}
		if (qNodes.empty())
			isOk = true;
	}
	unsigned int pos = 0;
	if (!genes.empty())
		pos = (--genes.end())->first + 1;
	auto &gene = genes[pos];
	gene.toNode = to;
	gene.fromNode = from;
	gene.innovationNum = pos;
	gene.weight = weight(gen);
}

void Genome::ConnectionEnableMutation() {
	std::uniform_real_distribution<double> rand(0.0, 1.0);
	std::uniform_int_distribution<unsigned int> randGene(0, genes.size());

	if (rand(gen) < mutationRates.enableMutationChance) {
		genes[randGene(gen)].enabled = true;
	}
	if (rand(gen) < mutationRates.disableMutationChance) {
		genes[randGene(gen)].enabled = false;
	}
}

void Genome::NodeMutation() {
	std::uniform_real_distribution<double> rand(0.0, 1.0);
	std::uniform_int_distribution<unsigned int> randGene(0, genes.size());

	if (rand(gen) < mutationRates.nodeMutationChance) {
		maxNeuron++;
		unsigned int disableNode = randGene(gen);

		genes[disableNode].enabled = false;
		unsigned int pos = 0;
		if (!genes.empty())
			pos = (--genes.end())->first + 1;
		auto &gene1 = genes[pos];
		gene1.toNode = maxNeuron;
		gene1.fromNode = genes[disableNode].fromNode;
		gene1.innovationNum = pos;
		gene1.weight = 1.0;

		pos = 0;
		if (!genes.empty())
			pos = (--genes.end())->first + 1;
		auto &gene2 = genes[pos];
		gene2.toNode = genes[disableNode].toNode;
		gene2.fromNode = maxNeuron;
		gene2.innovationNum = pos;
		gene2.weight = genes[disableNode].weight;

	}

}