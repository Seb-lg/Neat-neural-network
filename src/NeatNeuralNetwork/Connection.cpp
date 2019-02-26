//
// Created by seb on 24/02/19.
//

#include "Connection.hpp"

Connection::Connection():
	innovationNum(0),
	fromNode(0),
	toNode(0),
	weight(0.0),
	enabled(true)
{}

Connection::Connection(Connection const &other) {
	this->innovationNum = other.innovationNum;
	this->fromNode = other.fromNode;
	this->toNode = other.toNode;
	this->weight = other.weight;
	this->enabled = other.enabled;
}

void Connection::save(std::ofstream &stream) {
	stream << this->innovationNum << ' ';
	stream << this->fromNode << ' ';
	stream << this->toNode << ' ';
	stream << this->weight << ' ';
	stream << this->enabled << std::endl;
}

void Connection::load(std::ifstream &stream) {
	stream >> this->innovationNum;
	stream >> this->fromNode;
	stream >> this->toNode;
	stream >> this->weight;
	stream >> this->enabled;
}