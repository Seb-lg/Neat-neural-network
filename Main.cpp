//
// Created by seb on 21/01/19.
//

#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include "src/Snake/Generation.hpp"

int main() {
	Generation generation(2000);

	sf::VideoMode mode;
	mode.width = 1000;
	mode.height = 1000;
	mode.bitsPerPixel = 32;
	sf::RenderWindow *window;
	window = new sf::RenderWindow(mode, "Game", sf::Style::Resize | sf::Style::Close);
	window->setFramerateLimit(120);


	float width = 1000 / WIDTH;
	sf::CircleShape apple(width/2);
	apple.setFillColor(sf::Color(255, 0, 0));
	sf::CircleShape snake(width/2);
	snake.setFillColor(sf::Color(0, 255, 0));

	std::vector<std::vector<double>> dnas;


	sf::Event event;
	while (window->isOpen()) {
		while (generation.anyoneAlive()) {
			while (window->pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					window->close();
			}
			generation.update();
			auto it = generation.getChampion();

			for (int y = 0; y < WIDTH; y++) {
				for (int x = 0; x < WIDTH; x++) {
					if (it.getMap()[x][y] == 'A') {
						apple.setPosition(x * width, y * width);
						window->draw(apple);
					}
				}
			}
			snake.setPosition(it.x * width, it.y * width);
			window->draw(snake);

			window->display();
			window->clear(sf::Color::Black);
			std::this_thread::sleep_for(std::chrono::microseconds(16666));
		}
		//dnas.emplace_back(generation.getChampion().brain.getDNA());
		generation.newGen();
	}
}