//
// Created by seb on 21/01/19.
//

#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <Genome.hpp>
#include "src/ThreadPool/ThreadPool.hpp"


int main(int ac, char**av) {
	std::random_device rd;
	std::mt19937 genr(rd());

	NetworkInfo info;
	info.outputSize = 4;
	info.biasSize = 4;
	info.inputSize = 24;
	MutationRate rates;

	std::vector<std::unique_ptr<Genome>> population;

	population.reserve(1);
	for (int i = 0; i < 1; i++) {
		population.emplace_back(new Genome(info, rates));
		if (av[1])
			population[i]->load(av[1]);
	}

	std::uniform_int_distribution<unsigned long> rdParent(0, 10);
//	std::uniform_int_distribution<unsigned long> rdParent(0, 24);
	std::uniform_real_distribution<double> coin(0.0, 1.0);
	unsigned int gen = 1;
	unsigned int score = 0;
	/*ThreadPool POOL(2);
	while (gen != 0) {

		for (unsigned long pos = 10; pos < population.size(); pos++) {
			if (coin(genr) > 0.0)
				population[pos]->Crossover(*population[rdParent(genr)], *population[rdParent(genr)]);
			else
				population[pos]->Crossover(*population[rdParent(genr)]);
//			population[pos]->Mutate();
		}

		for (auto &item : population)
//		{
			item->Mutate();
//			item->fitness = item->run() + item->getSnakeSize();
//		}


		POOL.lockWork();
		for (auto it = population.begin(); it != population.end(); it++) {
			POOL.addTask([it](){
				(*it)->fitness = (*it)->run() + (*it)->getSnakeSize();
			});
		}
		POOL.unlockWork();
		while (!POOL.isDone())
			std::this_thread::yield();


		std::sort(population.begin(), population.end(), [](std::unique_ptr<Genome> const &a, std::unique_ptr<Genome> const &b) {
			return a->fitness > b->fitness;
		});
		if (population[0]->fitness > score) {
			population[0]->save(gen);
			population[0]->runGraphical();
		}
			score = population[0]->fitness;
//		std::cout << "left:\t"<< population[0]->nodes[0 + 16]->value << std::endl;
//		std::cout << "left up:\t"<< population[0]->nodes[1 + 16]->value << std::endl;
//		std::cout << "up:\t"<< population[0]->nodes[2 + 16]->value << std::endl;
//		std::cout << "up right:\t"<< population[0]->nodes[3 + 16]->value << std::endl;
//		std::cout << "right:\t"<< population[0]->nodes[4 + 16]->value << std::endl;
//		std::cout << "right down:\t"<< population[0]->nodes[5 + 16]->value << std::endl;
//		std::cout << "down:\t"<< population[0]->nodes[6 + 16]->value << std::endl;
//		std::cout << "down left:\t"<< population[0]->nodes[7 + 16]->value << std::endl;
		std::cout << std::endl;
		gen++;
		std::cout << gen << '\t' << population[0]->fitness << '\t' <<  population[0]->genes.size() << std::endl << std::endl;
	}*/

	population[0]->runGraphical();

	return 0;
}