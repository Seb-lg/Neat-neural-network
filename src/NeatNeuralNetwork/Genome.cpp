//
// Created by seb on 23/01/19.
//

#include "Genome.hpp"
#include <iostream>
#include <queue>
#include <map>
#include <list>
#include <thread>
#ifndef SERVER
#include "../Graphical.hpp"
static Graphical gr(50, 750);
#endif

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

void Genome::graphicalTic(sf::Color color) {
#ifndef SERVER
	gr.draw(getMap(), color);
	gr.update();
	std::this_thread::sleep_for(std::chrono::microseconds(16666));
#endif
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
			itM++;
		} else if (itM == mere.genes.end()) {
			this->genes.emplace(itP->first, new Connection(*itP->second));
			itP++;
		} else if (itP->second->innovationNum == itM->second->innovationNum) {
			this->genes.emplace(itP->second->innovationNum, new Connection(*itP->second));
			this->genes[itP->first]->weight = (coinFlip(getGen()) ? itP->second->weight : itM->second->weight);
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
	static unsigned int id = 35;

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

	std::map<unsigned int, std::vector<unsigned int>, std::greater<>>	calculation;

	for (unsigned int i = networkInfo.inputSize + networkInfo.biasSize; i < networkInfo.inputSize + networkInfo.biasSize + networkInfo.outputSize; i++) {
		calculation[0].emplace_back(i);
	}

	for (unsigned int pos = 0; !calculation[pos].empty(); pos++) {
		for (auto &item : calculation[pos]) {
			nodes[item]->Update(calculation, genes, networkInfo, pos);
		}
	}

	for (auto it = calculation.begin(); it != calculation.end(); ++it) {
		for (auto &item : it->second) {
			for (const auto &gene : genes) {
				if (gene.second->enabled && gene.second->toNode == item) {
					nodes[item]->value += nodes[gene.second->fromNode]->value * gene.second->weight;
				}
			}
			nodes[item]->value = nodes[item]->value / (1 + std::abs(nodes[item]->value));
		}
	}
}

void Genome::Mutate() {
	std::uniform_real_distribution<double> rand(0.0, 1.0);
	double p;

	NodeMutation();
	p = mutationRates.weightMutationChance;
	while (p > 0.0) {
		if (rand(getGen()) < p)
			WeightMutation();
		p = p - 1.0;
	}

	p = mutationRates.connectionMutateChance;
	while (p > 0.0) {
		if (rand(getGen()) < p)
			ConnectionMutate();
		p = p - 1.0;
	}

	ConnectionEnableMutation();

	
}

void Genome::Crossover(Genome &pere, Genome &mere) {
	std::uniform_int_distribution<int> coinFlip(0, 1);

	if (pere.fitness < mere.fitness) {
		this->Crossover(mere, pere);
	}

	this->genes.clear();
	this->nodes.clear();

	this->mutationRates = pere.mutationRates;
	this->networkInfo = pere.networkInfo;

	auto itP = pere.genes.begin();
	auto itM = mere.genes.begin();

	while (itP != pere.genes.end() || itM != mere.genes.end()) {
		if (itP == pere.genes.end()) {
			//this->genes.emplace(itM->first, new Connection(*itM->second));
			itM++;
		} else if (itM == mere.genes.end()) {
			if (itP->second->enabled)
				this->genes.emplace(itP->first, new Connection(*itP->second));
			itP++;
		} else if (itP->second->innovationNum == itM->second->innovationNum) {
			if (itP->second->enabled) {
				this->genes.emplace(itP->second->innovationNum, new Connection(*itP->second));
				this->genes[itP->first]->weight = (coinFlip(getGen()) ? itP->second->weight : itM->second->weight);
			}
			itP++;
			itM++;
		} else {
			if (itP->second->innovationNum < itM->second->innovationNum) {
				if (coinFlip(getGen()) && itP->second->enabled)
					this->genes.emplace(itP->first, new Connection(*itP->second));
				itP++;
			} else {
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

void Genome::Crossover(Genome &clone) {
	this->genes.clear();
	this->nodes.clear();
	this->connectionNet.clear();

	this->mutationRates = clone.mutationRates;
	this->networkInfo = clone.networkInfo;

	for (const auto &item : clone.genes) {
		if (item.second->enabled) {
			this->genes.emplace(item.second->innovationNum, new Connection(*item.second));
			this->connectionNet[item.second->fromNode].emplace_back(item.second->toNode);
		}
	}
	for (const auto &item : clone.nodes) {
		this->nodes.emplace(item.second->id, new Node(*item.second)).first->second->value = 1.0;
	}


	//std::cout << this->nodes.size() << " " << this->genes.size() << std::endl;
	//std::cout << clone.nodes.size() << " " << clone.genes.size() << std::endl << std::endl;
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
		connectionNet[tmp.fromNode].emplace_back(tmp.toNode);
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
	nodes[0 + 16]->value = 1.0;
	nodes[0 + 16]->value -= dist / mapSize;	/// LEFT


	dist = (head.first < head.second ? head.first : head.second);
	nodes[1 + 16]->calculated = true;
	nodes[1 + 16]->value = 1.0;
	nodes[1 + 16]->value -= dist / mapSize;	/// LEFT-UP


	dist = head.second;
	nodes[2 + 16]->calculated = true;
	nodes[2 + 16]->value = 1.0;
	nodes[2 + 16]->value -= dist / mapSize;	/// UP

	dist = head.second;
	dist = (dist < mapSize - head.first - 1 ? dist : mapSize - head.first - 1);
	nodes[3 + 16]->calculated = true;
	nodes[3 + 16]->value = 1.0;
	nodes[3 + 16]->value -= dist / mapSize;	/// UP-RIGHT


	dist = mapSize - head.first - 1;
	nodes[4 + 16]->calculated = true;
	nodes[4 + 16]->value = 1.0;
	nodes[4 + 16]->value -= dist / mapSize;	/// RIGHT


	dist = mapSize - (head.first > head.second ? head.first : head.second) - 1;
	nodes[5 + 16]->calculated = true;
	nodes[5 + 16]->value = 1.0;
	nodes[5 + 16]->value -= dist / mapSize;	/// RIGHT-DOWN


	dist = mapSize - head.second- 1;
	nodes[6 + 16]->calculated = true;
	nodes[6 + 16]->value = 1.0;
	nodes[6 + 16]->value -= dist / mapSize;	/// DOWN

	dist = head.first;
	dist = (dist < mapSize - head.second - 1 ? dist : mapSize - head.second - 1);
	nodes[7 + 16]->calculated = true;
	nodes[7 + 16]->value = 1.0;
	nodes[7 + 16]->value -= dist / mapSize;	/// DOWN-LEFT
}

void Genome::WeightMutation() {
	std::uniform_real_distribution<double> rand(0.0, 1.0);
	std::uniform_real_distribution<double> weight(-1.0, 1.0);
	std::uniform_real_distribution<double> step(-(mutationRates.stepSize/2), (mutationRates.stepSize/2));
	std::uniform_int_distribution<unsigned int> geneRD(0, genes.size() - 1);

	if (genes.empty())
		return;

	unsigned int tmp = geneRD(getGen());
	if (rand(getGen()) < mutationRates.perturbChance){
		std::next(genes.begin(), tmp)->second ->weight = weight(getGen());
	} else {
		std::next(genes.begin(), tmp)->second ->weight += step(getGen());
	}
}

void Genome::ConnectionMutate() {
	std::uniform_real_distribution<double> weight(-1.0, 1.0);
	std::uniform_real_distribution<double> randDO(0.0, 1.0);
	std::uniform_int_distribution<unsigned int> randNode(0, nodes.size() - 1);

	std::queue<unsigned int> qnode;
	unsigned int from;
	unsigned int to;
	unsigned int tmp;
	bool looping = true;


	while (looping){
		to = (randNode(getGen()) % (nodes.size() - 1 - networkInfo.inputSize - networkInfo.biasSize)) + networkInfo.inputSize + networkInfo.biasSize;
		from = randNode(getGen()) % (nodes.size() - 1 - networkInfo.outputSize);
		if (from >= networkInfo.inputSize + networkInfo.biasSize)
			from += networkInfo.outputSize;
		int loop = 0;
		while (from == to || std::find(connectionNet[from].begin(), connectionNet[from].end(), to) != connectionNet[from].end()) {
			from = randNode(getGen()) % (nodes.size() - 1 - networkInfo.outputSize);
			if (from >= networkInfo.inputSize + networkInfo.biasSize)
				from += networkInfo.outputSize;
			to = (randNode(getGen()) % (nodes.size() - 1 - networkInfo.inputSize - networkInfo.biasSize)) + networkInfo.inputSize + networkInfo.biasSize;
			if (++loop > 1000)
				return;
		}


		from = std::next(nodes.begin(), from)->second->id;
		to = std::next(nodes.begin(), to)->second->id;

//		std::cout << "from : " << from << "\tto : " << to << std::endl;

		connectionNet[from].emplace_back(to);
		while (!qnode.empty())
			qnode.pop();

		for (auto const &item : connectionNet[from]) {
			qnode.push(item);
			while (!qnode.empty()) {
				tmp = qnode.front();
				if (tmp == from)
					break;
				qnode.pop();
				for (auto const &nextn : connectionNet[tmp])
					qnode.push(nextn);
			}
			if (!qnode.empty())
				break;
		}
		if (qnode.empty())
			looping = false;

		connectionNet[from].erase(std::find(connectionNet[from].begin(), connectionNet[from].end(), to));
	}


	unsigned int pos = Genome::GetInnovation();
	auto gene = genes.emplace(pos, new Connection()).first;
	gene->second->toNode = to;
	gene->second->fromNode = from;
	gene->second->innovationNum = pos;
	gene->second->weight = weight(getGen());

	connectionNet[from].emplace_back(to);
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

		std::advance(it, disableNode);

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