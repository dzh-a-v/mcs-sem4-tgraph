#pragma once
#include "include/Graph.h"
#include <vector>
#include <optional>
#include <iostream>

//nullopt = unreachable
using WeightTable = std::vector<std::vector<std::optional<double>>>;

struct ShimbellOutput {
    WeightTable minWeights;
    WeightTable maxWeights;
    int edgeCount;
};

void printWeightTable(std::ostream& out, const WeightTable& table, 
                      const std::vector<int>& vertexIds);

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

    int mapVertexToIndex(int vertexId) const; // ФтФ
};
