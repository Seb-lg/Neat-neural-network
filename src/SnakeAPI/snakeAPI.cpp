
#include "snakeAPI.hpp"
#include <random>
#include <iterator>
#include <cstring>

std::size_t SnakeAPI::runGraphical(sf::Color color)
{
    _points = 0;
    _food = 250;
    generateMap();

    do {
        graphicalTic(color); // if you don't want to have graphical print, just do an empty function.
    } while (nextTic(computeDirection()));
    return _points;
}

std::size_t SnakeAPI::run()
{
    _points = 0;
    _food = 250;
    generateMap();

    do {
        // if you don't want to have graphical print, just do an empty function.
    } while (nextTic(computeDirection()));
    return _points;
}

bool SnakeAPI::nextTic(Direction dir)
{
    auto nPos = _snake[0];

    switch (dir) {
        case Direction::Up:
            nPos.second--;
            break;
        case Direction::Down: 
            nPos.second++;
            break;
        case Direction::Left:
            nPos.first--;
            break;
        case Direction::Right:
            nPos.first++;
            break;
    }
    if (nPos.second < 0 || nPos.second >= 50 || nPos.first < 0 || nPos.first >= 50)
    	return false;
    if (_map[nPos.second][nPos.first] == snake) {
        return false;
    }
    if (_map[nPos.second][nPos.first] == apple) {
        if (!generateNewApple())
            return false;
        _snake.push_back(_snake.back());
        _food += 250;
    }
    _map[_snake.back().second][_snake.back().first] = nothing;
    std::memmove(_snake.data() + 1, _snake.data(), (_snake.size() - 1) * sizeof(_snake[0]));
    _snake[0] = nPos;
    _map[nPos.second][nPos.first] = snake;
    _food--;
    _points++;
    if (_food == 0)
        return false;
    return true;
}

void SnakeAPI::generateMap(void)
{
    _snake.clear();
    for (posUnit_t y = 0; y < mapSize; ++y) {
        for (posUnit_t x = 0; x < mapSize; ++x) {
            _map[y][x] = nothing;
        }
    }
    _map[0][0] = snake;
    _map[0][1] = snake;
    _map[0][2] = snake;
    _map[0][3] = snake;
    _snake.push_back(std::make_pair(3, 0));
    _snake.push_back(std::make_pair(2, 0));
    _snake.push_back(std::make_pair(1, 0));
    _snake.push_back(std::make_pair(0, 0));
    generateNewApple();
//    _map[20][20] = apple;
}

bool SnakeAPI::generateNewApple(void)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::vector<pos_t> allPos;

    for (posUnit_t y = 0; y < mapSize; ++y) {
        for (posUnit_t x = 0; x < mapSize; ++x) {
            if (_map[y][x] == nothing)
                allPos.push_back(std::make_pair(x, y));
        }
    }
    if (allPos.size() == 0) {
        return false;
    }
    auto it = allPos.begin();
    std::uniform_int_distribution<> dis(0, std::distance(it, allPos.end()) - 1);
    std::advance(it, dis(gen));
    _map[it->second][it->first] = apple;
    _applePos = *it;
    return true;
}