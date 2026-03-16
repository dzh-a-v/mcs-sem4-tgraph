#pragma once
#include "include/Graph.h"
#include <vector>

/// Result of BFS traversal
struct BFSResult {
    std::vector<int> traversalOrder;  // Order of visited vertices
    int iterations;                    // Number of iterations for comparison
};

class BreadthFirstSearch {
public:
    explicit BreadthFirstSearch(const AdjacencyGraph& graph);
    
    /// Perform BFS traversal starting from a given vertex
    BFSResult traverse(int startVertex);
    
    /// Perform BFS and return vertices in level order
    BFSResult traverseLevels(int startVertex);

private:
    const AdjacencyGraph& m_graph;
};
