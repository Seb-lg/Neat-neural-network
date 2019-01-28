//
// Created by seb on 21/01/19.
//

#include <iostream>
#include "Snake.hpp"

Snake::Snake(): brain(std::vector<int>{8,6,4}), orient(Orientation::Right), fitness(0), x(WIDTH/2), y(WIDTH/2), dead(false), food(50) {
	map.resize(WIDTH);
	for (int x = 0; x < WIDTH; x++) {
		map[x].resize(WIDTH, ' ');
	}
}

Snake::Snake(std::vector<double> &DNA): brain(std::vector<int>{8,6,4}, DNA), orient(Orientation::Right), fitness(0), x(WIDTH/2), y(WIDTH/2), dead(false), food(50) {
	map.resize(WIDTH);
	for (int x = 0; x < WIDTH; x++) {
		map[x].resize(WIDTH, ' ');
	}
}


void Snake::update() {
	if (dead)
		return;
	auto ins = brain.getInputs();

	/// lidar UP
	ins[0].setActive(1/(static_cast<double>(y)));
	/// lidar LEFT
	ins[1].setActive(1/(static_cast<double>(x)));
	/// lidar RIGHT
	ins[2].setActive(1/(static_cast<double>(WIDTH - x)));
	/// lidar DOWN
	ins[3].setActive(1/(static_cast<double>(WIDTH - y)));

	ins[4].setActive(checkAppleUp());
	ins[5].setActive(checkAppleLeft());
	ins[6].setActive(checkAppleRight());
	ins[7].setActive(checkAppleDown());

	brain.update();

	auto outs = brain.getOutputs();


	/*std::cout << outs[0].getActive() << " ";
	std::cout << outs[1].getActive() << " ";
	std::cout << outs[2].getActive() << " ";
	std::cout << outs[3].getActive() << std::endl;*/

	double max = 0;
	int maxIndex = 0;
	for (int i = 0; i < outs.size(); i++) {
		if (max < outs[i].getActive()) {
			max = outs[i].getActive();
			maxIndex = i;
		}
	}

	if (maxIndex == 0) {
		orient = Orientation::Up;
	}
	if (maxIndex == 1) {
		orient = Orientation::Left;
	}
	if (maxIndex == 2) {
		orient = Orientation::Down;
	}
	if (maxIndex == 3) {
		orient = Orientation::Right;
	}

	switch (orient) {
		case Orientation::Up:
			y--;
			break;
		case Orientation::Right:
			x++;
			break;
		case Orientation::Down:
			y++;
			break;
		case Orientation::Left:
			x--;
			break;
	}

	if (x < 0 || y < 0 || x >= WIDTH || y >= WIDTH) {
		dead = true;
	}
	if (!dead && map[x][y] == 'A') {
		fitness += 50;
		food += 50;
	}
	if (food == 0)
		dead = true;
	if (!dead) {
		fitness++;
		food--;
	}

	for (auto &row : map) {
		for (auto &item : row) {
			if (item == 'S') {
				item = ' ';
				break;
			}
		}
	}
	if (!dead) {
		map[x][y] = 'S';
		apple();
	}
}

double Snake::checkAppleUp() {
	int i;
	for (i = 0; i < WIDTH; ++i) {
		if (y - i < 0)
			return (0.0);
		if (map[x][y - i] == 'A')
			return (1.0/ static_cast<double>(i));
	}
	return 0.0;
}

double Snake::checkAppleDown() {
	int i;
	for (i = 0; i < WIDTH; ++i) {
		if (y + i >= WIDTH)
			return (0.0);
		if (map[x][y + i] == 'A')
			return (1.0/ static_cast<double>(i));
	}
	return 0.0;
}

double Snake::checkAppleLeft() {
	int i;
	for (i = 0; i < WIDTH; ++i) {
		if (x - i < 0)
			return (0.0);
		if (map[x - i][y] == 'A')
			return (1.0/ static_cast<double>(i));
	}
	return 0.0;
}

double Snake::checkAppleRight() {
	int i;
	for (i = 0; i < WIDTH; ++i) {
		if (x + i >= WIDTH)
			return (0.0);
		if (map[x + i][y] == 'A')
			return (1.0/ static_cast<double>(i));
	}
	return 0.0;
}

void Snake::apple() {
	bool found = false;
	for (auto &row : map) {
		for (auto &item : row) {
			if (item == 'A') {
				found = true;
				break;
			}
		}
	}
	if (!found) {

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(0, WIDTH - 1);
		auto i = dist(gen);
		auto j = dist(gen);
		map[i][j] = 'A';
	}
}

int &Snake::getFitness() {
	return fitness;
}

int &Snake::getX() {
	return x;
}

int &Snake::getY() {
	return y;
}

bool &Snake::isDead() {
	return dead;
}

Brain &Snake::getBrain() {
	return brain;
}

const std::vector<std::vector<char>> &Snake::getMap() const {
	return map;
}

Orientation &Snake::getOrient() {
	return orient;
}
