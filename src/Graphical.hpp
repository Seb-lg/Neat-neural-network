//
// Created by seb on 12/02/19.
//

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "SnakeAPI/snakeAPI.hpp"

class Graphical {
public:
	Graphical() = delete;

	Graphical(int size, int width);

	void draw(SnakeAPI::map_t const &left, SnakeAPI::map_t const &right);
	void draw(SnakeAPI::map_t const &tableau);

	void update();

	inline bool isOpen() {
		return window.isOpen();
	}

private:
	sf::RenderWindow 	window;
	sf::CircleShape		circle;
	int 			widthMap;
	float 			widthCircle;
};

