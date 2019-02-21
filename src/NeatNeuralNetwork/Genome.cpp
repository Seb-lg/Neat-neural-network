//
// Created by seb on 23/01/19.
//

#include "Genome.hpp"
#include <iostream>
#include <queue>
#include <map>

auto &getGen() {
	static std::random_device rd;
	static std::mt19937 gen(rd());

	return (gen);
}

SnakeAPI::Direction Genome::computeDirection() const {
	return SnakeAPI::Direction::Right;
}

Genome::Genome(NetworkInfo const &info, MutationRate const &rates) {
	mutationRates = rates;
	networkInfo = info;
	maxNeuron = networkInfo.functionalNodes;

	for (unsigned int i = 0; i <= networkInfo.inputSize + networkInfo.biasSize + networkInfo.outputSize; i++){
		nodes.emplace(i, new Node(i));
	}

}

Genome &Genome::operator=(const Genome &other)
{
	this->mutationRates = other.mutationRates;
	this->networkInfo = other.networkInfo;
	for (auto const &e : other.genes)
		this->genes.emplace(e.first, new Connection(*e.second));
	for (auto const &e : other.nodes)
		this->nodes.emplace(e.first, new Node(*e.second));
	this->maxNeuron = other.maxNeuron;

	return (*this);
}


Genome::Genome(const Genome &other) {
	*this = other;
}

Genome::Genome(Genome &pere, Genome &mere) {
	std::uniform_int_distribution<int> coinFlip(0, 1);

	this->mutationRates = pere.mutationRates;
	this->networkInfo = pere.networkInfo;

	auto itP = pere.genes.begin();
	auto itM = mere.genes.begin();

	while (itP != pere.genes.end() && itM != mere.genes.end()) {
		if (itP == pere.genes.end()) {
			this->genes.emplace(itM->first, new Connection(*itM->second));
			itM++;
		} else if (itM == mere.genes.end()) {
			this->genes.emplace(itP->first, new Connection(*itP->second));
			itP++;
		} else if (itP->second->innovationNum == itM->second->innovationNum) {
			if (pere.fitness == mere.fitness) {
//				this->genes[itP->second->innovationNum] = std::rand() % 2 ? itP->second : itM->second;
				this->genes.emplace(itP->second->innovationNum, new Connection((coinFlip(getGen()) ? *itP->second : *itM->second)));
			} else if (pere.fitness > mere.fitness) {
				this->genes.emplace(itP->second->innovationNum, new Connection(*itP->second));
			} else {
				this->genes.emplace(itM->second->innovationNum, new Connection(*itM->second));
			}
			itP++;
			itM++;
		} else {
			if (itP->second->innovationNum < itM->second->innovationNum) {
				this->genes.emplace(itP->first, new Connection(*itP->second));
				itP++;
			} else {
				this->genes.emplace(itM->first, new Connection(*itM->second));
				itM++;
			}
		}
	}
	for (const auto &gene : genes) {
		nodes.try_emplace(gene.second->fromNode, new Node(gene.second->fromNode));
		nodes.try_emplace(gene.second->toNode, new Node(gene.second->toNode));
	}
}

unsigned int Genome::GetInnovation() {
	static unsigned int innovation = 0;

	return (innovation++);
}

unsigned int Genome::GetNodeId() {
	static unsigned int id = 32;

	return (id++);
}

void Genome::Update() {
	for (auto &node : nodes) {
		node.second->calculated = false;
	}

	for (unsigned int i = networkInfo.inputSize + networkInfo.biasSize; i < networkInfo.inputSize + networkInfo.biasSize + networkInfo.outputSize; i++) {
		nodes[i]->Update(genes, nodes, networkInfo);
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
			this->genes.emplace(itM->first, new Connection(*itM->second));
			itM++;
		} else if (itM == mere.genes.end()) {
			this->genes.emplace(itP->first, new Connection(*itP->second));
			itP++;
		} else if (itP->second->innovationNum == itM->second->innovationNum) {
			if (pere.fitness == mere.fitness) {
//				this->genes[itP->second->innovationNum] = std::rand() % 2 ? itP->second : itM->second;
				this->genes.emplace(itP->second->innovationNum, new Connection((coinFlip(getGen()) ? *itP->second : *itM->second)));
			} else if (pere.fitness > mere.fitness) {
				this->genes.emplace(itP->second->innovationNum, new Connection(*itP->second));
			} else {
				this->genes.emplace(itM->second->innovationNum, new Connection(*itM->second));
			}
			itP++;
			itM++;
		} else {
			if (itP->second->innovationNum < itM->second->innovationNum) {
				this->genes.emplace(itP->first, new Connection(*itP->second));
				itP++;
			} else {
				this->genes.emplace(itM->first, new Connection(*itM->second));
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
		if (rand(getGen()) < mutationRates.weightMutationChance) {
			if (rand(getGen()) < mutationRates.perturbChance){
				gene.second->weight = weight(getGen());
			} else {
				gene.second->weight += step(getGen());
			}

		}
	}
}

void Genome::ConnectionMutate() {
	std::uniform_real_distribution<double> randDO(0.0, 1.0);
	if (randDO(getGen()) > mutationRates.connectionMutateChance)
		return;
	std::uniform_int_distribution<unsigned int> rand(0, (nodes.size() == 0 ? 0: nodes.size() - 1));
	std::uniform_real_distribution<double> weight(-1.0, 1.0);
	bool isOk = false;
	unsigned int from;
	unsigned int to;

	std::map<unsigned int, std::vector<unsigned int>> connections;
	for (auto &gene : genes)
		connections[gene.second->fromNode].push_back(gene.second->toNode);

	std::vector<unsigned int> qNodes;

	while (!isOk) {
		unsigned int tmpN1 = rand(getGen());
		unsigned int tmpN2 = rand(getGen());
		while (tmpN1 == tmpN2)
			tmpN2 = rand(getGen());
		auto it = nodes.begin();

		std::advance(it, tmpN1);
		from = it->first;


		it = nodes.begin();
		std::advance(it, tmpN2);
		to = it->first;

		for (unsigned int item = 0; item < connections[to].size(); item++) {
			qNodes.push_back(connections[to][item]);
			while (!qNodes.empty()) {
				unsigned int tmp = qNodes.back();
				qNodes.pop_back();
				if (tmp == from) {
					qNodes.push_back(tmp);
					break;
				}
				for (auto &newN : connections[tmp])
					qNodes.push_back(newN);
			}
			if (!qNodes.empty()) {
				break;
			}
		}
		if (qNodes.empty())
			isOk = true;
	}
	unsigned int pos = Genome::GetInnovation();
	genes.emplace(pos, new Connection());
	auto &gene = genes[pos];
	gene->toNode = to;
	gene->fromNode = from;
	gene->innovationNum = pos;
	gene->weight = weight(getGen());
}

void Genome::ConnectionEnableMutation() {
	std::uniform_real_distribution<double> rand(0.0, 1.0);
	std::uniform_int_distribution<unsigned int> randGene(0, (genes.size() == 0 ? 0 : genes.size() - 1));

	if (!genes.empty() && rand(getGen()) < mutationRates.enableMutationChance) {
		auto it = genes.begin();
		unsigned int node = randGene(getGen());

		for (unsigned int i = 0; i < node && it != genes.end(); i++, it++);
		it->second->enabled = true;
	}
	if (!genes.empty() && rand(getGen()) < mutationRates.disableMutationChance) {
		auto it = genes.begin();
		unsigned int node = randGene(getGen());

		for (unsigned int i = 0; i < node && it != genes.end(); i++, it++);
		it->second->enabled = false;
	}
}

void Genome::NodeMutation() {
	std::uniform_real_distribution<double> rand(0.0, 1.0);
	std::uniform_int_distribution<unsigned int> randGene(0, (genes.size() == 0 ? 0 : genes.size() - 1));

	if (!genes.empty() && rand(getGen()) < mutationRates.nodeMutationChance) {
		maxNeuron++;
		unsigned int disableNode = randGene(getGen());

		auto it = genes.begin();

		for (unsigned int i = 0; i < disableNode && it != genes.end(); i++, it++);

		it->second->enabled = false;

		unsigned int pos = Genome::GetInnovation();
		unsigned int node = Genome::GetNodeId();
		nodes.emplace(node, new Node(node));

		genes.emplace(pos, new Connection());
		auto &gene1 = genes[pos];
		gene1->fromNode = it->second->fromNode;
		gene1->toNode = node;
		gene1->innovationNum = pos;
		gene1->weight = 1.0;

		pos = Genome::GetInnovation();
		genes.emplace(pos, new Connection());
		auto &gene2 = genes[pos];
		gene2->fromNode = node;
		gene2->toNode = it->second->toNode;
		gene2->innovationNum = pos;
		gene2->weight = it->second->weight;

	}

}