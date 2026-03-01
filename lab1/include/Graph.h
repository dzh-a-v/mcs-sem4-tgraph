#pragma once
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>

namespace graph {

/// Represents a weighted edge connecting two vertices
struct Edge {
    int from;
    int to;
    double weight;
};

/// Graph class supporting both directed and undirected weighted graphs.
/// Uses adjacency list representation for efficient neighbor lookups.
class Graph {
public:
    /// Constructs a graph. Set directed=true for directed graph, false for undirected.
    explicit Graph(bool directed = true);
    
    /// Adds a vertex with the given ID if it doesn't already exist.
    void addVertex(int id);
    
    /// Adds a weighted edge between two vertices. Automatically adds vertices if missing.
    /// For undirected graphs, adds edges in both directions.
    void addEdge(int from, int to, double weight);
    
    /// Returns a list of all vertex IDs in the graph.
    [[nodiscard]] std::vector<int> vertexIds() const;
    
    /// Returns all edges in the graph. For undirected graphs, each edge appears once (u < v).
    [[nodiscard]] std::vector<Edge> edges() const;
    
    /// Returns all neighbors of a vertex with their edge weights.
    [[nodiscard]] std::vector<std::pair<int, double>> neighbors(int v) const;
    
    /// Returns the weight of edge (from, to) if it exists, nullopt otherwise.
    [[nodiscard]] std::optional<double> getEdgeWeight(int from, int to) const;
    
    /// Checks if a vertex with the given ID exists in the graph.
    [[nodiscard]] bool hasVertex(int id) const;
    
    /// Returns true if the graph is directed, false if undirected.
    [[nodiscard]] bool isDirected() const;
    
    /// Returns the number of vertices in the graph.
    [[nodiscard]] int size() const;

private:
    bool m_directed;
    std::vector<int> m_vertices;
    std::unordered_map<int, std::vector<std::pair<int, double>>> m_adj;
};
}