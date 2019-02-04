cmake_minimum_required(VERSION 3.13)
project(Neat_neural_network)

set(CMAKE_CXX_STANDARD 17)

include_directories(src/NeatNeuralNetwork)
include_directories(src/Snake)

add_executable(Neat_neural_network
        src/NeatNeuralNetwork/Genome.cpp
        src/NeatNeuralNetwork/Genome.hpp
        src/Snake/Generation.cpp
        src/Snake/Generation.hpp
        src/Snake/Snake.cpp
        src/Snake/Snake.hpp
        Main.cpp)

target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-audio.so)
target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-graphics.so)
target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-window.so)
target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-system.so)