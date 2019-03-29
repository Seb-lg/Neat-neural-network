cmake_minimum_required(VERSION 3.10)
project(Neat_neural_network)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-g -Wall")

include_directories(src)
include_directories(src/Snake)
include_directories(src/NeatNeuralNetwork)

add_executable(Neat_neural_network
        src/ThreadPool/ThreadPool.cpp
        src/ThreadPool/ThreadPool.hpp

        src/NeatNeuralNetwork/Genome.cpp
        src/NeatNeuralNetwork/Genome.hpp

        src/NeatNeuralNetwork/Node.cpp
        src/NeatNeuralNetwork/Node.hpp

        src/NeatNeuralNetwork/NeatInfos.hpp

        src/NeatNeuralNetwork/Connection.cpp
        src/NeatNeuralNetwork/Connection.hpp

        src/SnakeAPI/snakeAPI.cpp
        src/SnakeAPI/snakeAPI.hpp

        src/Graphical.cpp
        src/Graphical.hpp

        Main.cpp src/NeatNeuralNetwork/Specie.cpp src/NeatNeuralNetwork/Specie.hpp)

target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-graphics.so)
target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-window.so)
target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-system.so)
target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libpthread.so)