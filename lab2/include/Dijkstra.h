#pragma once
#include "include/Graph.h"
#include <vector>
#include <optional>
#include <map>

/// Result of Dijkstra's algorithm
struct DijkstraResult {
    std::map<int, double> distances;      // Shortest distances from start
    std::map<int, int> predecessors;       // For path reconstruction
    std::vector<int> shortestPath;         // Reconstructed path to target
    double targetDistance;                 // Distance to target vertex
    int iterations;                        // Number of iterations for comparison
};

class DijkstraAlgorithm {
public:
    explicit DijkstraAlgorithm(const AdjacencyGraph& graph);
    
    /// Find shortest paths from start vertex to all others
    /// If targetVertex is provided, also reconstruct the path
    DijkstraResult findShortestPaths(int startVertex, std::optional<int> targetVertex = std::nullopt);

private:
    const AdjacencyGraph& m_graph;
    
    /// Reconstruct path from start to target using predecessors
    std::vector<int> reconstructPath(int startVertex, int targetVertex,
                                      const std::vector<int>& H,
                                      const std::vector<int>& vertexIds) const;
};
