//
// Created by seb on 24/02/19.
//

#include "Node.hpp"
#include <iostream>
#include <algorithm>


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

void Node::Update(std::map<unsigned int, std::vector<unsigned int>, std::greater<>> &calculation, std::map<unsigned int, std::unique_ptr<Connection>> &genes, NetworkInfo const &info, unsigned int pos) {
	if (id < info.inputSize + info.biasSize || calculated)
		return;
	value = 0.0;
	decltype(calculation.begin()->second.begin()) dele;
	for (const auto &gene : genes) {
		if (gene.second->enabled && gene.second->toNode == id) {
			calculation[pos + 1].emplace_back(gene.second->fromNode);
			for (auto it = calculation.begin(); it->first <= pos + 1; ++it) {
				if ((dele = std::find(it->second.begin(), it->second.end(), gene.second->fromNode)) != it->second.end())
					it->second.erase(dele);
			}
		}
	}
//	value = value / (1 + std::abs(value));
}