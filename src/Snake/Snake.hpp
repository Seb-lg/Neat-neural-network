//
// Created by seb on 21/01/19.
//

#ifndef AI_SNAKE_HPP
#define AI_SNAKE_HPP

#include <vector>
#include "Brain.hpp"

enum class Orientation {
	Up,
	Down,
	Left,
	Right
};

#define WIDTH 50

class Snake {
public:
	Snake();

	void update();
	void apple();

	double checkAppleUp();
	double checkAppleLeft();
	double checkAppleDown();
	double checkAppleRight();

	Brain		brain;
	Orientation	orient;
	int		fitness;
	int		x;
	int 		y;
	bool 		dead;
	int 		food;

	std::vector<std::vector<char>> map;
public:
	const std::vector<std::vector<char>> &getMap() const;
};


#endif //AI_SNAKE_HPP
