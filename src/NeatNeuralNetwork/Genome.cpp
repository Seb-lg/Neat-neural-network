//
// Created by seb on 23/01/19.
//

#include "Genome.hpp"
#include "../Graphical.hpp"
#include "../../../.CLion2018.3/system/.remote/62.210.76.136_22/2424eed3-37c6-45b5-a747-61d3ec9c2d45/usr/include/c++/7/bits/locale_facets.tcc"
#include <iostream>
#include <queue>
#include <map>
#include <list>
#include <thread>

auto &getGen() {
	static std::random_device rd;
	static std::mt19937 gen(rd());

	return (gen);
}

SnakeAPI::Direction Genome::computeDirection() {
	int max = 0;
	double maxVal = -2.0;
	this->Update();

	for (unsigned int i = 0; i < networkInfo.outputSize; i++) {
		if (nodes[i + networkInfo.inputSize + networkInfo.biasSize]->value > maxVal) {
			max = i;
			maxVal = nodes[i + networkInfo.inputSize + networkInfo.biasSize]->value;
		}
	}

	switch (max) {
		case 0:
			return SnakeAPI::Direction::Up;
		case 1:
			return SnakeAPI::Direction::Right;
		case 2:
			return SnakeAPI::Direction::Left;
		case 3:
			return SnakeAPI::Direction::Down;
		default:
			std::cout << "IMPOSSIBLE MOVE" << std::endl;
	}
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

	for (unsigned int j = networkInfo.inputSize; j < networkInfo.inputSize + networkInfo.biasSize; j++) {
		nodes[j]->value = 1.0;
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

	while (itP != pere.genes.end() || itM != mere.genes.end()) {
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
	for (unsigned int i = 0; i < networkInfo.inputSize + networkInfo.biasSize + networkInfo.outputSize; i++){
		nodes.emplace(i, new Node(i));
	}
	for (const auto &gene : genes) {
		nodes.try_emplace(gene.second->fromNode, new Node(gene.second->fromNode));
		nodes.try_emplace(gene.second->toNode, new Node(gene.second->toNode));
	}
}

void Genome::save(unsigned int generation) {
	std::ofstream output;
	std::string file;
	file = "./hall_of_champions/Champion_" + std::to_string(generation) + "_fit_" + std::to_string(fitness) + ".txt";
	output.open(file);
	if (!output.is_open()){
		std::cerr << "Couldn't open file '" << file << "' !";
		return ;
	}
	output << genes.size() << std::endl;
	for (const auto &gene : genes) {
		gene.second->save(output);
	}
}

void Genome::load(std::string file) {
	std::ifstream input;
	unsigned int size;
	Connection tmp;

	input.open(file);
	if (!input.is_open()){
		std::cerr << "Couldn't open file '" << file << "' !";
		return ;
	}
	input >> size;
	for (unsigned int i = 0; i <= size; i++) {
		tmp.load(input);
		genes.emplace(tmp.innovationNum, new Connection(tmp));
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
	for (long i = 1; head.first - i >= 0 ; ++i) {
		if (getMap()[head.second][head.first - i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[0]->value -= dist / mapSize;	/// LEFT


	dist = mapSize;
	nodes[1]->calculated = true;
	nodes[1]->value = 1.0;			/// LEFT-UP
	for (long i = 1; head.first - i >= 0 && head.second - i >= 0 ; ++i) {
		if (getMap()[head.second - i][head.first - i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[1]->value -= dist / mapSize;	/// LEFT-UP


	dist = mapSize;
	nodes[2]->calculated = true;
	nodes[2]->value = 1.0;			/// UP
	for (long i = 1; head.second - i >= 0 ; ++i) {
		if (getMap()[head.second - i][head.first] == apple) {
			dist = i;
			break;
		}
	}
	nodes[2]->value -= dist / mapSize;	/// UP


	dist = mapSize;
	nodes[3]->calculated = true;
	nodes[3]->value = 1.0;			/// UP-RIGHT
	for (long i = 1; head.first + i < mapSize && head.second + i < mapSize ; ++i) {
		if (getMap()[head.second + i][head.first + i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[3]->value -= dist / mapSize;	/// UP-RIGHT


	dist = mapSize;
	nodes[4]->calculated = true;
	nodes[4]->value = 1.0;			/// RIGHT
	for (long i = 1; head.first + i < mapSize ; ++i) {
		if (getMap()[head.second][head.first + i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[4]->value -= dist / mapSize;	/// RIGHT

	dist = mapSize;
	nodes[5]->calculated = true;
	nodes[5]->value = 1.0;			/// RIGHT-DOWN
	for (long i = 1; head.first + i < mapSize && head.second + i < mapSize ; ++i) {
		if (getMap()[head.second + i][head.first + i] == apple) {
			dist = i;
			break;
		}
	}
	nodes[5]->value -= dist / mapSize;	/// RIGHT-DOWN


	dist = mapSize;
	nodes[6]->calculated = true;
	nodes[6]->value = 1.0;			/// DOWN
	for (long i = 1; head.second + i < mapSize ; ++i) {
		if (getMap()[head.second + i][head.first] == apple) {
			dist = i;
			break;
		}
	}
	nodes[6]->value -= dist / mapSize;	/// DOWN

	dist = mapSize;
	nodes[7]->calculated = true;
	nodes[7]->value = 1.0;			/// DOWN-LEFT
	for (long i = 1; head.first - i >= 0 && head.second + i < mapSize ; ++i) {
		if (getMap()[head.second + i][head.first - i] == apple) {
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
	for (long i = 1; head.first - i >= 0 ; ++i) {
		if (getMap()[head.second][head.first - i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[0 + 8]->value -= dist / mapSize;	/// LEFT


	dist = mapSize;
	nodes[1 + 8]->calculated = true;
	nodes[1 + 8]->value = 1.0;		/// LEFT-UP
	for (long i = 1; head.first - i >= 0 && head.second - i >= 0 ; ++i) {
		if (getMap()[head.second - i][head.first - i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[1 + 8]->value -= dist / mapSize;	/// LEFT-UP


	dist = mapSize;
	nodes[2 + 8]->calculated = true;
	nodes[2 + 8]->value = 1.0;		/// UP
	for (long i = 1; head.second - i >= 0 ; ++i) {
		if (getMap()[head.second - i][head.first] == snake) {
			dist = i;
			break;
		}
	}
	nodes[2 + 8]->value -= dist / mapSize;	/// UP


	dist = mapSize;
	nodes[3 + 8]->calculated = true;
	nodes[3 + 8]->value = 1.0;		/// UP-RIGHT
	for (long i = 1; head.first + i < mapSize && head.second + i < mapSize ; ++i) {
		if (getMap()[head.second + i][head.first + i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[3 + 8]->value -= dist / mapSize;	/// UP-RIGHT


	dist = mapSize;
	nodes[4 + 8]->calculated = true;
	nodes[4 + 8]->value = 1.0;		/// RIGHT
	for (long i = 1; head.first + i < mapSize ; ++i) {
		if (getMap()[head.second][head.first + i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[4 + 8]->value -= dist / mapSize;	/// RIGHT

	dist = mapSize;
	nodes[5 + 8]->calculated = true;
	nodes[5 + 8]->value = 1.0;		/// RIGHT-DOWN
	for (long i = 1; head.first + i < mapSize && head.second + i < mapSize ; ++i) {
		if (getMap()[head.second + i][head.first + i] == snake) {
			dist = i;
			break;
		}
	}
	nodes[5 + 8]->value -= dist / mapSize;	/// RIGHT-DOWN


	dist = mapSize;
	nodes[6 + 8]->calculated = true;
	nodes[6 + 8]->value = 1.0;		/// DOWN
	for (long i = 1; head.second + i < mapSize ; ++i) {
		if (getMap()[head.second + i][head.first] == snake) {
			dist = i;
			break;
		}
	}
	nodes[6 + 8]->value -= dist / mapSize;	/// DOWN

	dist = mapSize;
	nodes[7 + 8]->calculated = true;
	nodes[7 + 8]->value = 1.0;		/// DOWN-LEFT
	for (long i = 1; head.first - i >= 0 && head.second + i < mapSize ; ++i) {
		if (getMap()[head.second + i][head.first - i] == snake) {
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
//	std::cout << "enter connection mutate " << genes.size() << std::endl;
	std::uniform_int_distribution<unsigned int> randfrom(0, nodes.size() - 1 - networkInfo.outputSize);
	std::uniform_int_distribution<unsigned int> randto(0, nodes.size() - 1 - networkInfo.inputSize - networkInfo.biasSize);
	std::uniform_real_distribution<double> weight(-1.0, 1.0);
	bool isOk = false;
	unsigned int from;
	unsigned int to;

	std::map<unsigned int, std::vector<unsigned int>> connections;
	for (auto &nd : nodes)
		connections[nd.first];
	for (auto &gene : genes) {
//		if (connections.find(gene.first) == connections.end())
//			std::cout << "FUUUUCK" << std::endl;
		connections[gene.second->fromNode].push_back(gene.second->toNode);
	}

	std::vector<unsigned int> qNodes;
	while (!isOk) {
		unsigned int tmpfrom = randfrom(getGen());
		unsigned int tmpto = randto(getGen()) + networkInfo.inputSize + networkInfo.biasSize;


		if (tmpfrom > networkInfo.inputSize + networkInfo.biasSize)
			tmpfrom += networkInfo.outputSize;

		while (tmpfrom == tmpto) {
			tmpto = randto(getGen()) + networkInfo.inputSize + networkInfo.biasSize;
		}

		auto it = nodes.begin();
		std::advance(it, tmpfrom);
		from = it->first;


		it = nodes.begin();
		std::advance(it, tmpto);
		to = it->first;

		qNodes.clear();
		for (unsigned int item = 0; item < connections[to].size(); item++) {
			qNodes.push_back(connections[to][item]);
			while (!qNodes.empty()) {
				unsigned int tmp = qNodes.back();
				qNodes.pop_back();
				if (tmp == from) {
					qNodes.push_back(tmp);
					break;
				}
				if (qNodes.size() >= 99) {
					std::cout << "WAW" << std::endl;
				}
				for (auto &newN : connections[tmp])
					if (connections.find(newN) != connections.end())
						qNodes.emplace_back(newN);
			}
			if (!qNodes.empty()) {
				break;
			}
		}
		if (qNodes.empty())
			isOk = true;
	}

	unsigned int pos = Genome::GetInnovation();
	auto gene = genes.emplace(pos, new Connection()).first;
	gene->second->toNode = to;
	gene->second->fromNode = from;
	gene->second->innovationNum = pos;
	gene->second->weight = weight(getGen());
//	std::cout << "exit connection mutate " << genes.size() << std::endl;
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