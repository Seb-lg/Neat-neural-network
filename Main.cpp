//
// Created by seb on 21/01/19.
//

#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include "Specie.hpp"

int main(int ac, char**av) {
	std::uniform_real_distribution<double> weight(-1.0, 1.0);

	NetworkInfo info;
	info.outputSize = 4;
	info.biasSize = 4;
	info.inputSize = 24;
	MutationRate rates;
	SpeciatingParameter specie;

	std::vector<std::shared_ptr<Specie>> species;

	species.reserve(20);
	for (int i = 0; i < 20; i++) {
		species.emplace_back(new Specie(specie, info, rates));
	}

	unsigned int gen = 1;
	while (gen != 0) {

		for (auto &item : species)
			item->Update();


		POOL.lockWork();
		for (auto it = species.begin(); it != species.end(); it++) {
			for (auto &item : species)
				item->Run();
		}
		POOL.unlockWork();
		while (!POOL.isDone())
			std::this_thread::yield();


		/*if (population[0]->fitness > score) {
			population[0]->save(gen);
			score = population[0]->fitness;
		}*/
		for (auto &item : species) {
			item->population[0]->runGraphical(item->color);
		}

		gen++;
	}

	return 0;
}