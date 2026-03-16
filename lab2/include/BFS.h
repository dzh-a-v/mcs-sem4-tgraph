#pragma once
#include "include/Graph.h"
#include <vector>
#include <map>

/// Result of BFS traversal with level information
struct BFSResult {
    std::vector<int> traversalOrder;      // Order of visited vertices
    std::map<int, std::vector<int>> levels; // Vertices grouped by level
    int iterations;                        // Number of iterations for comparison
    int maxLevel;                          // Maximum depth reached
};

class BreadthFirstSearch {
public:
    explicit BreadthFirstSearch(const AdjacencyGraph& graph);
    
    /// Perform BFS traversal starting from a given vertex with level tracking
    BFSResult traverseWithLevels(int startVertex);

private:
    const AdjacencyGraph& m_graph;
};
