#pragma once
#include "include/Graph.h"
#include <vector>
#include <optional>

namespace graph {
using DistanceMatrix = std::vector<std::vector<std::optional<double>>>;

struct ShimbellResult {
    DistanceMatrix min_distances;  
    DistanceMatrix max_distances; 
    int path_length; // Number of edges in the paths
};

/// Implements Shimbell's method for finding shortest and longest paths
/// of exactly k edges between all pairs of vertices.
/// Uses matrix multiplication approach over (min,+) and (max,+) semirings.
class ShimbellMethod {
public:
    explicit ShimbellMethod(const Graph& graph);
    ShimbellResult compute(int pathLength);

private:
    const Graph& m_graph_;
    std::vector<int> m_vertex_ids_;
    int m_size_;
    DistanceMatrix createAdjacencyMatrix() const;
    DistanceMatrix multiplyMin(const DistanceMatrix& a, const DistanceMatrix& b) const;
    DistanceMatrix multiplyMax(const DistanceMatrix& a, const DistanceMatrix& b) const;
    int getIndex(int vertexId) const;
};
}