//
// Created by seb on 21/01/19.
//

#ifndef AI_GENERATION_HPP
#define AI_GENERATION_HPP


#include <algorithm>
#include <iostream>
#include "Snake.hpp"

class Generation {
public:
	Generation(int population = 500): population(population), gen(0) {
		snakes.resize(population);
		map.resize(50);
		for (int x = 0; x < 50; x++) {
			map[x].resize(50, " ");
		}

	}

	void update() {
		for (auto &item : snakes) {
			item.update();
		}
	}

	bool anyoneAlive() {
		for (auto &snake : snakes) {
			if (!snake.isDead())
				return (true);
		}
		return false;
	}

	void show() {
		std::sort(snakes.begin(), snakes.end(), [](Snake i,Snake j) { return (i.getFitness()>j.getFitness()); });

		auto it = snakes.begin();
		while (it->isDead() && it != snakes.end())
			it++;

		if (it == snakes.end())
			return;

		for (int y = 0; y < WIDTH; y++) {
			for (int x = 0; x < WIDTH; x++) {
				std::cout << it->getMap()[x][y];
			}
			std::cout << "|" << std::endl;
		}
	}

	void newGen() {
		std::cout << "-------------------------------------------------------------------------------------" << std::endl;
		std::cout << "Generation : " << gen << std::endl;
		gen++;

		for (auto &item : snakes) {
			std::cout << item.getFitness() << std::endl;
		}

		int total = 0;
		int act = 0;


		for (auto &snake : snakes) {
			total += snake.getFitness();
		}
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(0, total);
		std::uniform_int_distribution<int> dist(0, 1);
		std::uniform_int_distribution<int> percent(0, 100);
		std::uniform_real_distribution<double> rdouble(-1.1, 1.0);
		std::vector<double> child;
		auto pereID = dis(gen);
		auto mereID = dis(gen);
		std::vector<double> pere;
		std::vector<double> mere;
		for (int i = 0; i < this->population; ++i) {
			act = 0;
			for (auto &snake : snakes) {
				act += snake.getFitness();
				if (act >= pereID) {
					pere = snake.getBrain().getDNA();
					break;
				}
			}
			act = 0;
			for (auto &snake : snakes) {
				act += snake.getFitness();
				if (act >= mereID) {
					mere = snake.getBrain().getDNA();
					break;
				}
			}

			child.resize(pere.size());
			auto dnap = pere.begin();
			auto dnac = child.begin();
			for (auto dna = mere.begin(); dna < mere.end(); ++dna, ++dnap, ++dnac) {
				if (dist(gen))
					*dnac = *dnap;
				else
					*dnac = *dna;
				if (percent(gen) < 10)
					*dnac = rdouble(gen);

			}
			snakes.emplace_back(child);
		}
	}

	Snake &getChampion() {
		std::sort(snakes.begin(), snakes.end(), [](Snake i,Snake j) { return (i.getFitness()>j.getFitness()); });
		return (snakes[0]);
	}

	std::vector<Snake> &getGeneration() {
		std::sort(snakes.begin(), snakes.end(), [](Snake i,Snake j) { return (i.getFitness()>j.getFitness()); });
		return (snakes);
	}

private:
	std::vector<Snake> snakes;
	std::vector<std::vector<std::string>> map;
	int 					population;
	int 					gen;
};


#endif //AI_GENERATION_HPP
