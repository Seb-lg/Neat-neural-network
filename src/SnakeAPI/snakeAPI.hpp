#pragma once

#include <vector>
#include <array>
#include <SFML/Graphics/Color.hpp>


class SnakeAPI {
public:
    virtual ~SnakeAPI() = default;

    enum class Direction {Up, Down, Left, Right};

    static constexpr char apple = 'A';
    static constexpr char snake = 'S';
    static constexpr char nothing = ' ';

    uint64_t                                _food;

//    using posUnit_t = std::size_t;
    using posUnit_t = long;
    static constexpr posUnit_t mapSize = 50;
    using pos_t = std::pair<posUnit_t, posUnit_t>; // (x, y)
    using map_t = std::array<std::array<char, mapSize>, mapSize>;

    uint64_t run(void); // return the points
    uint64_t runGraphical(sf::Color color); // return the points

    uint64_t getPoints(void) const {return _points;}
    unsigned long const getSnakeSize(void) const {return _snake.size();}

protected:

    virtual Direction computeDirection() = 0;
    virtual void graphicalTic(sf::Color color) = 0;

    map_t const &getMap(void) const {return _map;}
    pos_t const &getApplePos(void) const {return _applePos;}
    pos_t const &getHeadPos(void) const {return _snake[0];}

private:
    map_t                                   _map;
    std::vector<pos_t>                      _snake;
    pos_t                                   _applePos;
    uint64_t                                _points;
    bool                                    _end;

private:
    void generateMap(void);
    bool generateNewApple(void);
    bool nextTic(Direction);
};
