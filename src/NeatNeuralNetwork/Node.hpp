//
// Created by seb on 24/02/19.
//

#pragma once

#include <map>
#include <memory>
#include <queue>
#include "NeatInfos.hpp"
#include "Connection.hpp"

class Node {
public:
	Node();
	Node(const Node &other);
	Node(unsigned int id);


	void Update(std::map<unsigned int, std::vector<unsigned int>, std::greater<>> &calculation, std::map<unsigned int, std::unique_ptr<Connection>> &genes, NetworkInfo const &info, unsigned int pos);

	unsigned int	id;
	double		value;
	bool		calculated;
};
