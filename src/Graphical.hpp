//
// Created by seb on 12/02/19.
//

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>

class Graphical {
public:
	Graphical() = delete;

	Graphical(int size, int width, bool duo);

	void draw(std::vector<std::vector<char>> const &left, std::vector<std::vector<char>> const &right);
	void draw(std::vector<std::vector<char>> const &tableau);

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

