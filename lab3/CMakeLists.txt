cmake_minimum_required(VERSION 3.10)
project(lab2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(lab2
    src/main.cpp
    src/Graph.cpp
    src/Generator.cpp
    src/PathCounter.cpp
    src/ShimbellMethod.cpp
    src/Eccentricity.cpp
    src/BFS.cpp
    src/Dijkstra.cpp
    src/AlgorithmComparator.cpp
)
