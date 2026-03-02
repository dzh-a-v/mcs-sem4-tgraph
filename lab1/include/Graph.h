#pragma once
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>

/*
 * =============================================================================
 * GRAPH DATA STRUCTURE
 * =============================================================================
 * 
 * Represents a weighted graph G = (V, E) using adjacency list.
 * Supports both directed and undirected graphs.
 * 
 * Key Concepts:
 *   - Simple path: no repeated vertices
 *   - Cycle: path starting and ending at same vertex
 *   - Acyclic: no cycles present
 *   - Connected: path exists between any two vertices
 * =============================================================================
 */

/// Represents a single weighted edge
struct WeightedEdge {
    int from;       /// Source vertex ID
    int to;         /// Destination vertex ID
    double weight;  /// Edge weight/cost
};

/// Graph class with adjacency list representation
class AdjacencyGraph {
public:
    /// Create graph (directed by default)
    explicit AdjacencyGraph(bool directed = true);
    
    /// Add vertex if not exists
    void addVertex(int id);
    
    /// Add weighted edge (auto-adds vertices)
    void addEdge(int from, int to, double weight);
    
    /// Get all vertex IDs
    std::vector<int> vertexIds() const;
    
    /// Get all edges
    std::vector<WeightedEdge> edges() const;
    
    /// Get neighbors with weights
    std::vector<std::pair<int, double>> neighbors(int v) const;
    
    /// Get specific edge weight
    std::optional<double> getEdgeWeight(int from, int to) const;
    
    /// Check if vertex exists
    bool hasVertex(int id) const;
    
    /// Check if directed
    bool isDirected() const;
    
    /// Get vertex count
    int size() const;

private:
    bool m_directed;
    std::vector<int> m_vertices;
    std::unordered_map<int, std::vector<std::pair<int, double>>> m_adj;
};
