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

	NetworkInfo info;
	info.outputSize = 4;
	info.biasSize = 3;
	info.inputSize = 24;
	info.functionalNodes = info.outputSize + info.biasSize + info.inputSize;
	MutationRate rates;

	std::vector<Genome> population;

	population.reserve(100);
	for (int i = 0; i < 100; i++) {
		population.emplace_back(info, rates);
	}

	for (auto &item2 : population) {
		std::cout << item2.genes.size() << "\t" << item2.nodes.size() << std::endl;
	}

	std::cout << "---------------------------------------------------------------" << std::endl;


	/*ThreadPool<decltype(population.begin())> POOL([](decltype(population.begin()) gen){
		for (int i = 0; i < 100; i++)
			gen->Mutate();
	});

	POOL.lockWork();

	for (auto it = population.begin(); it != population.end(); it++) {
		POOL.addTask(it);
	}

	POOL.unlockWork();

	while(!POOL.isDone());

	POOL.~ThreadPool<decltype(population.begin())>();*/

	for (int i = 0; i < 100; i++)
		population.begin()->Mutate();

	for (auto &item2 : population) {
		std::cout << item2.genes.size() << "\t" << item2.nodes.size() << std::endl;
	}

	std::vector<std::vector<char>> map1;
	std::vector<std::vector<char>> map2;

	map1.resize(CARRE);
	for (auto &item : map1) {
		item.resize(CARRE, ' ');
	}

	map2.resize(CARRE);
	for (auto &item : map2) {
		item.resize(CARRE, ' ');
	}

	return 0;
	/*map1[0][0] = 'S';
	map1[1][0] = 'S';
	map1[0][1] = 'S';
	map1[CARRE - 1][CARRE - 1] = 'A';
	map1[CARRE - 1][0] = 'A';
	map2[0][0] = 'S';
	map2[CARRE - 1][CARRE - 1] = 'A';

	Graphical gr(CARRE, 750, true);


	while (gr.isOpen()) {
		gr.draw(map1, map2);
		gr.update();
	}
	*/
}