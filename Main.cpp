//
// Created by seb on 21/01/19.
//

#include <iostream>
#include <algorithm>
// #include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <Genome.hpp>
// #include "src/Graphical.hpp"
#include "src/ThreadPool/ThreadPool.hpp"

#define CARRE 50

int main() {
	std::random_device rd;
	std::mt19937 genr(rd());

	NetworkInfo info;
	info.outputSize = 4;
	info.biasSize = 3;
	info.inputSize = 24;
	MutationRate rates;

	std::vector<std::unique_ptr<Genome>> population;

	population.reserve(1000);
	for (int i = 0; i < 1000; i++) {
		population.emplace_back(new Genome(info, rates));
	}
	std::uniform_int_distribution<unsigned long> rdParent(0, population.size() / 2);
	unsigned int gen = 0;
	while (gen < 1000) {

		for (unsigned long pos = population.size() / 2; pos < population.size(); pos++) {
			population[pos]->Crossover(*population[rdParent(genr)], *population[rdParent(genr)]);
		}

		/*{
			ThreadPool<decltype(population.begin())> POOL([](decltype(population.begin()) gen) {
				(*gen)->Mutate();
				(*gen)->fitness = (*gen)->run();
			}, 1);
			POOL.lockWork();
			for (auto it = population.begin(); it != population.end(); it++) {
				POOL.addTask(it);
			}
			POOL.unlockWork();
			while (!POOL.isDone());
		}*/

		for (auto &item : population) {
			item->Mutate();
//			std::cout << item->genes.size() << std::endl;
			item->fitness = item->run();
		}

		std::sort(population.begin(), population.end(), [](std::unique_ptr<Genome> const &a, std::unique_ptr<Genome> const &b) {
			return a->fitness > b->fitness;
		});
		population[0]->save(gen);
		//population[0]->runGraphical();
		gen++;
		std::cout << population[0]->fitness << std::endl;
		std::cout << population[0]->genes.size() << std::endl << std::endl;
	}

	Genome test2(info, rates);

	std::string str;

	std::cout << "Enter a champion to load: ";
	std::cin >> str;

	test2.load(str);


	return 0;
}