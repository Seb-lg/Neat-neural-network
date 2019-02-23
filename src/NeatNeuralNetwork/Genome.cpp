//
// Created by seb on 23/01/19.
//

#include "Genome.hpp"
#include "../Graphical.hpp"
#include <iostream>
#include <queue>
#include <map>
#include <thread>

auto &getGen() {
	static std::random_device rd;
	static std::mt19937 gen(rd());

	return (gen);
}

SnakeAPI::Direction Genome::computeDirection() const {
	return SnakeAPI::Direction::Right;
}

void Genome::graphicalTic() {
	static Graphical gr(50, 750);

	gr.draw(getMap());
	gr.update();
	std::this_thread::sleep_for(std::chrono::microseconds(16666));
}

Genome::Genome(NetworkInfo const &info, MutationRate const &rates): SnakeAPI() {
	mutationRates = rates;
	networkInfo = info;
	maxNeuron = networkInfo.functionalNodes;

	for (unsigned int i = 0; i < networkInfo.inputSize + networkInfo.biasSize + networkInfo.outputSize; i++){
		nodes.emplace(i, new Node(i));
	}

}

Genome::Genome(const Genome &other): SnakeAPI() {
	*this = other;
}

Genome::Genome(Genome &pere, Genome &mere): SnakeAPI() {
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

	GetInApple();
	GetInBody();
	GetInWall();

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
	this->nodes.clear();

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

void Genome::GetInApple() {
	auto head = getHeadPos();
	double dist;

	dist = mapSize;
	nodes[0]->calculated = true;
	nodes[0]->value = 1.0;			/// LEFT
	for (unsigned long i = 1; head.first - i >= 0 ; ++i) {
		if (getMap()[head.first - i][head.second] == apple) {
			dist = i;
			break;
		}
	}
	nodes[0]->value -= dist / mapSize;	/// LEFT


	dist = mapSize;
	nodes[1]->calculated = true;
	nodes[1]->value = 1.0;			/// LEFT-UP
	for (unsigned long i = 1; head.first - i >= 0 && head.second - i >= 0 ; ++i) {
		if (getMap()[head.first - i][head.second - i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[1]->value -= dist / mapSize;	/// LEFT-UP


	dist = mapSize;
	nodes[2]->calculated = true;
	nodes[2]->value = 1.0;			/// UP
	for (unsigned long i = 1; head.second - i >= 0 ; ++i) {
		if (getMap()[head.first][head.second - i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[2]->value -= dist / mapSize;	/// UP


	dist = mapSize;
	nodes[3]->calculated = true;
	nodes[3]->value = 1.0;			/// UP-RIGHT
	for (unsigned long i = 1; head.first + i < mapSize && head.second + i < mapSize ; ++i) {
		if (getMap()[head.first + i][head.second + i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[3]->value -= dist / mapSize;	/// UP-RIGHT


	dist = mapSize;
	nodes[4]->calculated = true;
	nodes[4]->value = 1.0;			/// RIGHT
	for (unsigned long i = 1; head.first + i < mapSize ; ++i) {
		if (getMap()[head.first + i][head.second] == apple) {
			dist = i;
			break;
		}
	}
	nodes[4]->value -= dist / mapSize;	/// RIGHT

	dist = mapSize;
	nodes[5]->calculated = true;
	nodes[5]->value = 1.0;			/// RIGHT-DOWN
	for (unsigned long i = 1; head.first + i < mapSize && head.second + i < mapSize ; ++i) {
		if (getMap()[head.first + i][head.second + i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[5]->value -= dist / mapSize;	/// RIGHT-DOWN


	dist = mapSize;
	nodes[6]->calculated = true;
	nodes[6]->value = 1.0;			/// DOWN
	for (unsigned long i = 1; head.second + i < mapSize ; ++i) {
		if (getMap()[head.first][head.second + i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[6]->value -= dist / mapSize;	/// DOWN

	dist = mapSize;
	nodes[7]->calculated = true;
	nodes[7]->value = 1.0;			/// DOWN-LEFT
	for (unsigned long i = 1; head.first - i >= 0 && head.second + i < mapSize ; ++i) {
		if (getMap()[head.first - i][head.second + i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[7]->value -= dist / mapSize;	/// DOWN-LEFT
}

void Genome::GetInBody() {
	auto head = getHeadPos();
	double dist;

	dist = mapSize;
	nodes[0 + 8]->calculated = true;
	nodes[0 + 8]->value = 1.0;		/// LEFT
	for (unsigned long i = 1; head.first - i >= 0 ; ++i) {
		if (getMap()[head.first - i][head.second] == snake) {
			dist = i;
			break;
		}
	}
	nodes[0 + 8]->value -= dist / mapSize;	/// LEFT


	dist = mapSize;
	nodes[1 + 8]->calculated = true;
	nodes[1 + 8]->value = 1.0;		/// LEFT-UP
	for (unsigned long i = 1; head.first - i >= 0 && head.second - i >= 0 ; ++i) {
		if (getMap()[head.first - i][head.second - i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[1 + 8]->value -= dist / mapSize;	/// LEFT-UP


	dist = mapSize;
	nodes[2 + 8]->calculated = true;
	nodes[2 + 8]->value = 1.0;		/// UP
	for (unsigned long i = 1; head.second - i >= 0 ; ++i) {
		if (getMap()[head.first][head.second - i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[2 + 8]->value -= dist / mapSize;	/// UP


	dist = mapSize;
	nodes[3 + 8]->calculated = true;
	nodes[3 + 8]->value = 1.0;		/// UP-RIGHT
	for (unsigned long i = 1; head.first + i < mapSize && head.second + i < mapSize ; ++i) {
		if (getMap()[head.first + i][head.second + i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[3 + 8]->value -= dist / mapSize;	/// UP-RIGHT


	dist = mapSize;
	nodes[4 + 8]->calculated = true;
	nodes[4 + 8]->value = 1.0;		/// RIGHT
	for (unsigned long i = 1; head.first + i < mapSize ; ++i) {
		if (getMap()[head.first + i][head.second] == snake) {
			dist = i;
			break;
		}
	}
	nodes[4 + 8]->value -= dist / mapSize;	/// RIGHT

	dist = mapSize;
	nodes[5 + 8]->calculated = true;
	nodes[5 + 8]->value = 1.0;		/// RIGHT-DOWN
	for (unsigned long i = 1; head.first + i < mapSize && head.second + i < mapSize ; ++i) {
		if (getMap()[head.first + i][head.second + i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[5 + 8]->value -= dist / mapSize;	/// RIGHT-DOWN


	dist = mapSize;
	nodes[6 + 8]->calculated = true;
	nodes[6 + 8]->value = 1.0;		/// DOWN
	for (unsigned long i = 1; head.second + i < mapSize ; ++i) {
		if (getMap()[head.first][head.second + i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[6 + 8]->value -= dist / mapSize;	/// DOWN

	dist = mapSize;
	nodes[7 + 8]->calculated = true;
	nodes[7 + 8]->value = 1.0;		/// DOWN-LEFT
	for (unsigned long i = 1; head.first - i >= 0 && head.second + i < mapSize ; ++i) {
		if (getMap()[head.first - i][head.second + i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[7 + 8]->value -= dist / mapSize;	/// DOWN-LEFT
}

void Genome::GetInWall() {
	auto head = getHeadPos();
	double dist;

	dist = head.first;
	nodes[0 + 16]->calculated = true;
	nodes[0 + 16]->value -= dist / mapSize;	/// LEFT


	dist = (head.first < head.second ? head.first : head.second);
	nodes[1 + 16]->calculated = true;
	nodes[1 + 16]->value -= dist / mapSize;	/// LEFT-UP


	dist = head.second;
	nodes[2 + 16]->calculated = true;
	nodes[2 + 16]->value -= dist / mapSize;	/// UP

	dist = head.second;
	dist = (dist < mapSize - head.first - 1 ? dist : mapSize - head.first - 1);
	nodes[3 + 16]->calculated = true;
	nodes[3 + 16]->value -= dist / mapSize;	/// UP-RIGHT


	dist = mapSize - head.first - 1;
	nodes[4 + 16]->calculated = true;
	nodes[4 + 16]->value -= dist / mapSize;	/// RIGHT


	dist = mapSize - (head.first > head.second ? head.first : head.second) - 1;
	nodes[5 + 16]->calculated = true;
	nodes[5 + 16]->value -= dist / mapSize;	/// RIGHT-DOWN


	dist = mapSize - head.second- 1;
	nodes[6 + 16]->calculated = true;
	nodes[6 + 16]->value -= dist / mapSize;	/// DOWN

	dist = head.first;
	dist = (dist < mapSize - head.second - 1 ? dist : mapSize - head.second - 1);
	nodes[7 + 16]->calculated = true;
	nodes[7 + 16]->value -= dist / mapSize;	/// DOWN-LEFT
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