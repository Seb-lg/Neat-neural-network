
#include "snakeAPI.hpp"
#include <random>
#include <iterator>
#include <cstring>

std::size_t SnakeAPI::run(void)
{
    Direction dir;

    generateMap();

    do {
        /* you can set her a graphical interface */
    } while (nextTic(computeDirection()));
    return _points;
}

bool SnakeAPI::nextTic(Direction dir)
{
    auto nPos = _snake[0];

    switch (dir) {
        case Direction::Up:
            if (nPos.second-- == 0)
                return false;
            break;
        case Direction::Down: 
            if (nPos.second++ == mapSize - 1)
                return false;
            break;
        case Direction::Left:
            if (nPos.first-- == 0)
                return false;
            break;
        case Direction::Right:
            if (nPos.first++ == mapSize - 1)
                return false;
            break;
    }
    if (_map[nPos.second][nPos.first] == snake) {
        return false;
    }
    if (_map[nPos.second][nPos.first] == apple) {
        if (!generateNewApple())
            return false;
        _snake.push_back(_snake.back());
        ++_points;
    }
    _map[_snake.back().second][_snake.back().first] = nothing;
    std::memmove(_snake.data() + 1, _snake.data(), (_snake.size() - 1) * sizeof(_snake[0]));
    _snake[0] = nPos;
    _map[nPos.second][nPos.first] = nothing;
    return true;
}

void SnakeAPI::generateMap(void)
{
    for (posUnit_t y = 0; y < mapSize; ++y) {
        for (posUnit_t x = 0; x < mapSize; ++x) {
            _map[y][x] = nothing;
        }
    }
    _map[mapSize / 2][mapSize / 2 + 1] = snake;
    _map[mapSize / 2][mapSize / 2] = snake;
    _map[mapSize / 2][mapSize / 2 - 1] = snake;
    _snake.emplace_back(std::make_pair(mapSize / 2 + 1, mapSize / 2));
    _snake.emplace_back(std::make_pair(mapSize / 2, mapSize / 2));
    _snake.emplace_back(std::make_pair(mapSize / 2 - 1, mapSize / 2));
    generateNewApple();
}

bool SnakeAPI::generateNewApple(void)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::vector<pos_t> allPos;

    for (posUnit_t y = 0; y < mapSize; ++y) {
        for (posUnit_t x = 0; x < mapSize; ++x) {
            if (!_map[y][x])
                allPos.push_back(std::make_pair(x, y));
        }
    }
    if (allPos.empty()) {
        return false;
    }
    auto it = allPos.begin();
    std::uniform_int_distribution<> dis(0, std::distance(it, allPos.end()) - 1);
    std::advance(it, dis(gen));
    _map[it->second][it->first] = apple;
    _applePos = *it;
    return true;
}