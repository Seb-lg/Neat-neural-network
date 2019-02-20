cmake_minimum_required(VERSION 3.10)
project(Neat_neural_network)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-g -Wall")

include_directories(src/NeatNeuralNetwork)
include_directories(src/Snake)

add_executable(Neat_neural_network
        src/ThreadPool/ThreadPool.cpp
        src/ThreadPool/ThreadPool.hpp

        src/NeatNeuralNetwork/Genome.cpp
        src/NeatNeuralNetwork/Genome.hpp


        src/SnakeAPI/snakeAPI.cpp
        src/SnakeAPI/snakeAPI.hpp

        Main.cpp src/Snake/Snake.cpp src/Snake/Snake.hpp)
        #src/Graphical.cpp
        #src/Graphical.hpp

# target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-audio.so)
# target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-graphics.so)
# target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-window.so)
# target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libsfml-system.so)
target_link_libraries(Neat_neural_network /usr/lib/x86_64-linux-gnu/libpthread.so)