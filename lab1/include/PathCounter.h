#pragma once
#include "include/Graph.h"
#include <vector>

namespace graph {

/// Matrix type for storing multiple paths (each path is a vector of vertex IDs)
using PathMatrix = std::vector<std::vector<int>>;

/// Finds and counts all simple paths between two vertices in a graph.
/// Uses backtracking algorithm to enumerate all possible paths.
class PathCounter {
public:
    /// Constructs a PathCounter for the given graph.
    /// @param graph Reference to the graph to analyze
    explicit PathCounter(Graph const& graph);
    
    /// Returns the number of distinct paths from one vertex to another.
    /// @param from Starting vertex ID
    /// @param to Destination vertex ID
    /// @return Count of all simple paths
    [[nodiscard]] int getPathCount(int from, int to);
    
    /// Returns all simple paths from one vertex to another.
    /// @param from Starting vertex ID
    /// @param to Destination vertex ID
    /// @return Matrix containing all paths (each row is one path)
    [[nodiscard]] PathMatrix getAllPaths(int from, int to);
    
private:
    Graph const& m_graph_;
    
    /// Recursive backtracking helper to find all paths.
    /// @param current Current vertex in traversal
    /// @param target Destination vertex
    /// @param visited Tracking array for visited vertices in current path
    /// @param currentPath Path being constructed
    /// @param allPaths Collection of all complete paths found
    void backtrackAllPaths(int current, int target, std::vector<bool>& visited,
                          std::vector<int>& currentPath, PathMatrix& allPaths);
};
}