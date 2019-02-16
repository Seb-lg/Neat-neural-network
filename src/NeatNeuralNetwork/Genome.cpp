//
// Created by seb on 23/01/19.
//

#include "Genome.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>


Genome::Genome(NetworkInfo &info, MutationRate &rates): gen(rd()){
	mutationRates = rates;
	networkInfo = info;
	maxNeuron = networkInfo.functionalNodes;

	for (unsigned int i = 0; i <= networkInfo.inputSize + networkInfo.biasSize + networkInfo.outputSize; i++){
		nodes.emplace(i, i);
	}
}

Genome::Genome(const Genome &other): gen(rd()) {
	this->mutationRates = other.mutationRates;
	this->networkInfo = other.networkInfo;
	this->genes = other.genes;
	this->nodes = other.nodes;
	this->maxNeuron = other.maxNeuron;
}

Genome::Genome(Genome &pere, Genome &mere): gen(rd()) {
	std::uniform_int_distribution<int> coinFlip(0, 1);

	this->mutationRates = pere.mutationRates;
	this->networkInfo = pere.networkInfo;

	auto itP = pere.genes.begin();
	auto itM = mere.genes.begin();

	while (itP != pere.genes.end() && itM != mere.genes.end()) {
		if (itP == pere.genes.end()) {
			this->genes[itM->first] = itM->second;
			itM++;
		} else if (itM == mere.genes.end()) {
			this->genes[itP->first] = itP->second;
			itP++;
		} else if (itP->second.innovationNum == itM->second.innovationNum) {
			if (pere.fitness == mere.fitness) {
				this->genes[itP->second.innovationNum] = (coinFlip(gen) ? itP->second : itM->second);
			} else if (pere.fitness > mere.fitness) {
				this->genes[itP->second.innovationNum] = itP->second;
			} else {
				this->genes[itM->second.innovationNum] = itM->second;
			}
			itP++;
			itM++;
		} else {
			if (itP->second.innovationNum < itM->second.innovationNum) {
				this->genes[itP->first] = itP->second;
				itP++;
			} else {
				this->genes[itM->first] = itM->second;
				itM++;
			}
		}
	}
	for (const auto &gene : genes) {
		nodes.try_emplace(gene.second.fromNode, gene.second.fromNode);
		nodes.try_emplace(gene.second.toNode, gene.second.toNode);
	}
}

unsigned int Genome::GetInnovation() {
	static unsigned int innovation = 0;

	return (innovation++);
}

void Genome::Update() {
	for (auto &node : nodes) {
		node.second.calculated = false;
	}

	for (unsigned int i = networkInfo.inputSize + networkInfo.biasSize; i < networkInfo.inputSize + networkInfo.biasSize + networkInfo.outputSize; i++) {
		nodes[i].Update(genes, nodes, networkInfo);
	}
}

void Genome::Mutate() {
	WeightMutation();
	ConnectionMutate();
	ConnectionEnableMutation();
	NodeMutation();
}

void Genome::Crossover(Genome &pere, Genome &mere) {
	std::uniform_int_distribution<int> coinFlip(0, 1);

	this->genes.clear();

	this->mutationRates = pere.mutationRates;
	this->networkInfo = pere.networkInfo;

	auto itP = pere.genes.begin();
	auto itM = mere.genes.begin();

	while (itP != pere.genes.end() && itM != mere.genes.end()) {
		if (itP == pere.genes.end()) {
			this->genes[itM->first] = itM->second;
			itM++;
		} else if (itM == mere.genes.end()) {
			this->genes[itP->first] = itP->second;
			itP++;
		} else if (itP->second.innovationNum == itM->second.innovationNum) {
			if (pere.fitness == mere.fitness) {
				this->genes[itP->second.innovationNum] = (coinFlip(gen) ? itP->second : itM->second);
			} else if (pere.fitness > mere.fitness) {
				this->genes[itP->second.innovationNum] = itP->second;
			} else {
				this->genes[itM->second.innovationNum] = itM->second;
			}
			itP++;
			itM++;
		} else {
			if (itP->second.innovationNum < itM->second.innovationNum) {
				this->genes[itP->first] = itP->second;
				itP++;
			} else {
				this->genes[itM->first] = itM->second;
				itM++;
			}
		}
	}
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
	std::uniform_real_distribution<double> randDO(0.0, 1.0);
	if (randDO(gen) > mutationRates.connectionMutateChance)
		return;
	std::uniform_int_distribution<unsigned int> rand(0, maxNeuron);
	std::uniform_real_distribution<double> weight(-1.0, 1.0);
	bool isOk = false;
	unsigned int from;
	unsigned int to;

	std::unordered_map<unsigned int, std::vector<unsigned int>> connections;
	if (!connections.empty()) {
		connections[0].push_back(20);
		for (const auto &gene : genes)
			connections[gene.second.fromNode].push_back(gene.second.toNode);
	}

	std::queue<unsigned int> qNodes;
	from = rand(gen);
	to = rand(gen);
	while (from == to)
		to = rand(gen);

	while (!isOk && !connections.empty()) {
		from = rand(gen);
		to = rand(gen);

		for (unsigned int item = 0; item < connections[to].size(); item++) {
			qNodes.push(connections[to][item]);
			while (!qNodes.empty()) {
				unsigned int tmp = qNodes.front();
				if (tmp == from)
					break;
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
	gene.innovationNum = Genome::GetInnovation();
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

		unsigned int pos = Genome::GetInnovation();
		nodes.emplace(maxNeuron, maxNeuron);

		auto &gene1 = genes[pos];
		gene1.toNode = maxNeuron;
		gene1.fromNode = genes[disableNode].fromNode;
		gene1.innovationNum = pos;
		gene1.weight = 1.0;

		pos = Genome::GetInnovation();
		auto &gene2 = genes[pos];
		gene2.toNode = genes[disableNode].toNode;
		gene2.fromNode = maxNeuron;
		gene2.innovationNum = pos;
		gene2.weight = genes[disableNode].weight;

	}

}