#include "include/Generator.h"
#include <cmath>
#include <random>
#include <set>

namespace graph {

/// Generates a random weight using the Rayleigh distribution formula.
/// Uses inverse transform sampling: X = scale * sqrt(-2 * ln(1 - U))
/// where U is uniformly distributed in [0, 1).
double Generator::generateRayleighWeight(double scale) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return scale * std::sqrt(-2.0 * std::log(1.0 - dist(m_rng)));
}

/// Generates a random acyclic graph with the specified number of vertices and edges.
/// For directed graphs, ensures acyclicity by only allowing edges from lower to higher vertex IDs.
/// Edge weights are generated using Rayleigh distribution.
std::unique_ptr<Graph> Generator::generateAcyclicGraph(int vertices, int edges, bool directed, double scale) {
    auto graph = std::make_unique<Graph>(directed);
    for (int i = 0; i < vertices; ++i) graph->addVertex(i);

    std::set<std::pair<int, int>> existingEdges;
    int added = 0;
    while (added < edges) {
        int u = std::uniform_int_distribution<>(0, vertices - 1)(m_rng);
        int v = std::uniform_int_distribution<>(0, vertices - 1)(m_rng);

        if (u == v) continue;
        if (directed && u > v) std::swap(u, v);
        if (!directed && u > v) std::swap(u, v);

        if (existingEdges.find({u, v}) == existingEdges.end()) {
            graph->addEdge(u, v, generateRayleighWeight(scale));
            existingEdges.insert({u, v});
            added++;
        }
    }
    return graph;
}
}