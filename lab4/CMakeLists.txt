cmake_minimum_required(VERSION 3.10)
project(lab4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(lab4
    src/main.cpp
    src/Graph.cpp
    src/Generator.cpp
    src/PathCounter.cpp
    src/ShimbellMethod.cpp
    src/Eccentricity.cpp
    src/BFS.cpp
    src/Dijkstra.cpp
    src/AlgorithmComparator.cpp
    src/FlowNetwork.cpp
    src/FlowNetworkBuilder.cpp
    src/MaxFlowSolver.cpp
    src/MinCostFlow.cpp
    src/MermaidExporter.cpp
    src/Kirchhoff.cpp
    src/Kruskal.cpp
    src/PruferCode.cpp
    src/MaxMatching.cpp
)
