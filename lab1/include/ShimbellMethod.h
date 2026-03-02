#pragma once
#include "include/Graph.h"
#include <vector>
#include <optional>
#include <iostream>

//nullopt = unreachable
using WeightTable = std::vector<std::vector<std::optional<double>>>;

/// Result container for Shimbell computation
struct ShimbellOutput {
    WeightTable minWeights;
    WeightTable maxWeights;
    int edgeCount;
};

/// Print weight matrix to stream (for debugging/visualization)
void printWeightTable(std::ostream& out, const WeightTable& table, 
                      const std::vector<int>& vertexIds);

/// Print adjacency matrix to stream
void printAdjacencyMatrix(std::ostream& out, const AdjacencyMatrix& matrix, 
                          const std::vector<int>& vertexIds);

class KPathCalculator {
public:
    explicit KPathCalculator(const AdjacencyGraph& graph);
    ShimbellOutput compute(int edgeCount);

private:
    const AdjacencyGraph& m_graph;
    std::vector<int> m_vertexIds; // List of vertex IDs
    int m_vertexCount;

    WeightTable buildWeightMatrix() const;

    WeightTable multiplyMinPlus(const WeightTable& left, const WeightTable& right) const;

    WeightTable multiplyMaxPlus(const WeightTable& left, const WeightTable& right) const;

    // Map vertex ID to matrix index
    int mapVertexToIndex(int vertexId) const;
};
