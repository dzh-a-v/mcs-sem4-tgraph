#pragma once
#include "include/Graph.h"
#include <random>

namespace graph {

/// Generates random acyclic graphs with weighted edges.
/// Edge weights follow a Rayleigh distribution.
class Generator {
public:
    /// Generates a random acyclic graph with specified parameters.
    /// @param vertices Number of vertices in the graph
    /// @param edges Number of edges to create
    /// @param directed Whether the graph should be directed
    /// @param scale Scale parameter for Rayleigh distribution (default 1.0)
    /// @return Unique pointer to the generated Graph
    std::unique_ptr<Graph> generateAcyclicGraph(int vertices, int edges, bool directed, double scale = 1.0);
    
private:
    std::mt19937 m_rng{std::random_device{}()};
    
    /// Generates a random weight using Rayleigh distribution.
    /// @param scale Scale parameter of the distribution
    /// @return Random weight value
    double generateRayleighWeight(double scale);
};
}