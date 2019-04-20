//
// Created by seb on 26/03/19.
//

#include "Specie.hpp"

static std::random_device rd;
static std::mt19937 genr(rd());
static std::uniform_real_distribution<double> coin(0.0, 1.0);
static std::uniform_int_distribution<int> colorRd(0, 255);

Specie::Specie(SpeciatingParameter const &specie, NetworkInfo const &info, MutationRate const &rates): color(colorRd(genr), colorRd(genr), colorRd(genr), 255) {

	this->specie = specie;
	this->info = info;
	this->rates = rates;

	population.reserve(specie.population);
	for (unsigned int i = 0; i < specie.population; ++i) {
		population.emplace_back(new Genome(info, rates));
	}
}

void Specie::Run() {
	POOL.lockWork();
	for (auto &item : population) {
		POOL.addTask([item](){
			item->fitness = item->run() + item->getSnakeSize();
		});
	}
	POOL.unlockWork();
	while (!POOL.isDone())
		std::this_thread::yield();

}

void Specie::Update() {
	std::uniform_int_distribution<unsigned long> rdParent(0, static_cast<unsigned long>(specie.population / specie.parentPercent));

	for (unsigned long pos = static_cast<unsigned long>(specie.population / specie.parentPercent); pos < specie.population; pos++) {
		if (coin(genr) < 1.0)
			population[pos]->Crossover(*population[rdParent(genr)], *population[rdParent(genr)]);
		else
			population[pos]->Crossover(*population[rdParent(genr)]);
	}

	for (auto &item : population) {
		item->Mutate();
	}

	std::sort(population.begin(), population.end(), [](std::shared_ptr<Genome> const &a, std::shared_ptr<Genome> const &b) {
		return a->fitness > b->fitness;
	});

	fitness = 0;
	maxFitness = population[0]->fitness;
	minFitness = population[0]->fitness;

	for (const auto &item : population) {
		if (item->fitness > maxFitness)
			maxFitness = item->fitness;
		if (item->fitness < minFitness)
			minFitness = item->fitness;
		fitness += item->fitness;
	}
	fitness = fitness / population.size();
}