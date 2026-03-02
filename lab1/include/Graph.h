#pragma once
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>

namespace graph {

/*
 * =============================================================================
 * GRAPH DATA STRUCTURE
 * =============================================================================
 * 
 * A graph G = (V, E) consists of:
 *   - V: a set of vertices (nodes)
 *   - E: a set of edges connecting pairs of vertices
 * 
 * This implementation supports:
 *   - Directed graphs: edges have direction (u → v)
 *   - Undirected graphs: edges are bidirectional (u — v)
 *   - Weighted edges: each edge has an associated cost/distance
 * 
 * Representation: Adjacency List
 *   - Space complexity: O(|V| + |E|)
 *   - Neighbor lookup: O(degree(v))
 *   - More efficient than adjacency matrix for sparse graphs
 * 
 * Key Concepts:
 *   - Simple path: a path with no repeated vertices
 *   - Cycle: a path that starts and ends at the same vertex
 *   - Acyclic graph: a graph containing no cycles
 *   - Connected graph: there exists a path between any two vertices
 * =============================================================================
 */

/// Represents a weighted edge connecting two vertices
struct Edge {
    int from;
    int to;
    double weight;
};

/// Graph class supporting both directed and undirected weighted graphs.
/// Contains adjacency list.
class Graph {
public:
    explicit Graph(bool directed = true);
    void addVertex(int id);
    void addEdge(int from, int to, double weight);
    [[nodiscard]] std::vector<int> vertexIds() const;
    [[nodiscard]] std::vector<Edge> edges() const;
    [[nodiscard]] std::vector<std::pair<int, double>> neighbors(int v) const;
    [[nodiscard]] std::optional<double> getEdgeWeight(int from, int to) const;
    [[nodiscard]] bool hasVertex(int id) const;
    [[nodiscard]] bool isDirected() const;
    [[nodiscard]] int size() const;

private:
    bool m_directed;
    std::vector<int> m_vertices;
    std::unordered_map<int, std::vector<std::pair<int, double>>> m_adj;
    //  {        vertex_id:  {            neighbor_id: weight}        } 
};
}