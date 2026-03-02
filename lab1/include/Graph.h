#pragma once
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>

/// Represents a weighted edge
struct WeightedEdge {
    int from;
    int to;
    double weight;
};

/// Adjacency matrix type (true = edge exists, false = no edge)
using AdjacencyMatrix = std::vector<std::vector<bool>>;

class AdjacencyGraph {
public:
    explicit AdjacencyGraph(bool directed = true);
    void addVertex(int id);
    void addEdge(int from, int to, double weight); // Checks existence FtF
    std::vector<int> vertexIds() const;
    std::vector<WeightedEdge> edges() const;
    std::vector<std::pair<int, double>> neighbors(int v) const;
    std::optional<double> getEdgeWeight(int from, int to) const;
    bool hasVertex(int id) const; // FtF
    bool isDirected() const;
    int size() const;

    AdjacencyMatrix getAdjacencyMatrix() const;

private:
    bool m_directed;
    std::vector<int> m_vertices;
    std::unordered_map<int, std::vector<std::pair<int, double>>> m_adj;
};
