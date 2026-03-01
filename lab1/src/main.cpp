#include <iostream>
#include "include/Graph.h"
#include "include/Generator.h"
#include "include/ShimbellMethod.h"
#include "include/PathCounter.h"

using namespace graph;

int readInt(const std::string& prompt) {
    std::cout << prompt;
    int val; std::cin >> val;
    return val;
}

void printMatrix(const std::string& title, const DistanceMatrix& mat, const std::vector<int>& ids) {
    std::cout << "\n=== " << title << " ===\n";
    std::cout << "   ";
    for (int id : ids) std::cout << "v" << id << " ";
    std::cout << "\n";
    for (size_t i = 0; i < mat.size(); ++i) {
        std::cout << "v" << ids[i] << " ";
        for (size_t j = 0; j < mat[i].size(); ++j) {
            if (mat[i][j]) std::cout << mat[i][j].value() << " ";
            else std::cout << "inf ";
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "Laboratory Work #1\n";

    int vertices = readInt("Number of vertices: ");
    int edges = readInt("Number of edges: ");
    bool directed = readInt("Directed? (1 - yes, 0 - no): ") == 1;
    double rayleighScale = 1.0;

    Generator gen;
    auto graph = gen.generateAcyclicGraph(vertices, edges, directed, rayleighScale);

    std::cout << "[OK] Graph generated (V=" << vertices << ", E=" << edges << ")\n";

    int pathLen = readInt("Path length for Shimbell (k): ");
    ShimbellMethod shimbell(*graph);
    auto result = shimbell.compute(pathLen);

    auto ids = graph->vertexIds();
    printMatrix("Minimum paths (Shimbell)", result.min_distances, ids);
    printMatrix("Maximum paths (Shimbell)", result.max_distances, ids);

    int from = readInt("Start vertex: ");
    int to = readInt("End vertex: ");

    if (!graph->hasVertex(from) || !graph->hasVertex(to)) {
        std::cout << "[FAIL] Vertices do not exist\n";
        return 0;
    }

    PathCounter counter(*graph);
    auto paths = counter.getAllPaths(from, to);

    if (paths.empty()) {
        std::cout << "[FAIL] No paths found\n";
    } else {
        std::cout << "[OK] Paths found: " << paths.size() << "\n";
    }

    return 0;
}