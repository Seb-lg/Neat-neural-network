//
// Created by seb on 24/02/19.
//

#include "Node.hpp"

Node::Node() {
	this->id = 0;
	this->value = 0.0;
	this->calculated = false;
}

Node::Node(const Node &other) {
	this->id = other.id;
	this->value = other.value;
	this->calculated = other.calculated;
}

Node::Node(unsigned int id) {
	this->id = id;
	this->value = 0.0;
	this->calculated = false;
}

double Node::Update(std::map<unsigned int, std::unique_ptr<Connection>> &genes, std::map<unsigned int, std::unique_ptr<Node>> &nodes, NetworkInfo const &info) {
	if (id < info.inputSize + info.biasSize || calculated)
		return value;
	calculated = true;
	value = 0.0;
	for (const auto &gene : genes) {
		if (gene.second->toNode == id)
			value += nodes[gene.second->fromNode]->Update(genes, nodes, info) * gene.second->weight;
	}
	value = value / (1 + std::abs(value));
	return value;
}