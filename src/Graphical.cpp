//
// Created by seb on 12/02/19.
//

#include <SFML/Window/Event.hpp>
#include "Graphical.hpp"

Graphical::Graphical(int size, int width):
	window(sf::VideoMode(width * 2, width), "AI Battle", sf::Style::Close, sf::ContextSettings(0, 0, 10)),
	widthMap(size){

	widthCircle = static_cast<float>(width) / (size + 2);

	circle.setRadius(widthCircle/2 - 1.f);
	circle.setFillColor(sf::Color::Black);
	//circle.setOutlineThickness(1.f);
	//circle.setOutlineColor(sf::Color(211,211,211,255));
}

void Graphical::draw(SnakeAPI::map_t const &left, SnakeAPI::map_t const &right) {
	int i = 1;

	for (int y = 0; y < left.size(); y++) {
		for (int x = 0; x < left[x].size(); x++) {
			if (left[x][y] == 'S')
				circle.setFillColor(sf::Color::Green);
			else if (left[x][y] == 'A')
				circle.setFillColor(sf::Color::Red);
			else
				circle.setFillColor(sf::Color::Black);
			circle.setPosition((i + x) * widthCircle, (y + 1) * widthCircle);
			window.draw(circle);
		}
	}

	i = left.size() + 3;
	for (int y = 0; y < right.size(); y++) {
		for (int x = 0; x < right[x].size(); x++) {
			if (right[y][x] == 'S')
				circle.setFillColor(sf::Color::Green);
			else if (right[y][x] == 'A')
				circle.setFillColor(sf::Color::Red);
			else
				circle.setFillColor(sf::Color::Black);
			circle.setPosition((i + x) * widthCircle, (y + 1) * widthCircle);
			window.draw(circle);
		}
	}

	circle.setFillColor(sf::Color::White);
	for (int x = 0; x <= right.size() + left.size() + i; x++) {
		circle.setPosition((x) * widthCircle, (0) * widthCircle);
		window.draw(circle);
		circle.setPosition((x) * widthCircle, (1 + left.size()) * widthCircle);
		window.draw(circle);
	}

	for (int y = 0; y <= right.size() + 1; y++) {
		circle.setPosition((0) * widthCircle, (y) * widthCircle);
		window.draw(circle);
		circle.setPosition((left.size() + 1) * widthCircle, (y) * widthCircle);
		window.draw(circle);
		circle.setPosition((left.size() + 2) * widthCircle, (y) * widthCircle);
		window.draw(circle);
		circle.setPosition((right.size() + left.size() + 3) * widthCircle, (y) * widthCircle);
		window.draw(circle);
	}
}

void Graphical::draw(SnakeAPI::map_t const &tableau) {
	int i;

	i = tableau.size() / 2 + 3;
	for (int x = 0; x < tableau.size(); x++) {
		for (int y = 0; y < tableau[x].size(); y++) {
			if (tableau[y][x] == 'S')
				circle.setFillColor(sf::Color::Green);
			else if (tableau[y][x] == 'A')
				circle.setFillColor(sf::Color::Red);
			else
				circle.setFillColor(sf::Color::Black);
			circle.setPosition((i + x) * widthCircle, (y + 1) * widthCircle);
			window.draw(circle);
		}
	}
}

void Graphical::update() {
	sf::Event event;

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			window.close();
	}
	window.display();
	window.clear();
}