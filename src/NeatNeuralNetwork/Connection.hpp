//
// Created by seb on 24/02/19.
//

#pragma once

#include <fstream>

class Connection {
public:
	Connection();
	Connection(Connection const &other);

	void save(std::ofstream &stream);
	void load(std::ifstream &stream);

	unsigned int	innovationNum;
	unsigned int	fromNode;
	unsigned int	toNode;
	double		weight;
	bool		enabled;
};
