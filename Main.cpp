//
// Created by seb on 21/01/19.
//

#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <zconf.h>
#include "src/Graphical.hpp"

#define CARRE 50

int main() {

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

	map1[0][0] = 'S';
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

}