#pragma once
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>

/// Represents a single weighted edge
struct WeightedEdge {
    int from;   
    int to;  
    double weight;
};

/// Graph class with adjacency list representation
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

private:
    bool m_directed;
    std::vector<int> m_vertices;
    std::unordered_map<int, std::vector<std::pair<int, double>>> m_adj;
};
