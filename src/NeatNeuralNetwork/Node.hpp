//
// Created by seb on 24/02/19.
//

#pragma once

#include <map>
#include <memory>
#include "NeatInfos.hpp"
#include "Connection.hpp"

class Node {
public:
	Node();
	Node(const Node &other);
	Node(unsigned int id);

	double Update(std::map<unsigned int, std::unique_ptr<Connection>> &genes, std::map<unsigned int, std::unique_ptr<Node>> &nodes, NetworkInfo const &info);

	unsigned int	id;
	double		value;
	bool		calculated;
};
