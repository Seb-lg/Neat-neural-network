//
// Created by seb on 26/03/19.
//

#pragma once

#ifndef SERVER
#include <SFML/Graphics/Color.hpp>
#endif

#include <vector>
#include "Genome.hpp"
#include "ThreadPool/ThreadPool.hpp"

static ThreadPool POOL;

class Specie {
public:
	Specie() = delete;

	Specie(SpeciatingParameter const &specie, NetworkInfo const &info, MutationRate const &rates);

	void Run();
	void Update();

public:
	std::vector<std::shared_ptr<Genome>>	population;
	SpeciatingParameter			specie;
	NetworkInfo				info;
	MutationRate				rates;

#ifndef SERVER
	sf::Color				color;
#endif
};

