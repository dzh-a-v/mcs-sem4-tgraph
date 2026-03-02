#pragma once
#include "include/Graph.h"
#include <vector>

/*
 * =============================================================================
 * SIMPLE PATH ENUMERATION
 * =============================================================================
 * 
 * Finds ALL simple paths (no repeated vertices) between two vertices.
 * 
 * Algorithm: Depth-first search with backtracking
 *   1. Start at source vertex
 *   2. Mark as visited, add to current path
 *   3. If at destination: save path
 *   4. Otherwise: recurse on unvisited neighbors
 *   5. Backtrack: unmark, remove from path
 * 
 * Complexity: O(V! / (V-k)!) worst case
 * =============================================================================
 */

/// Matrix storing multiple paths (each row = one path)
using PathCollection = std::vector<std::vector<int>>;

/// Path counter using backtracking
class SimplePathFinder {
public:
    /// Initialize with graph
    explicit SimplePathFinder(AdjacencyGraph const& graph);
    
    /// Count paths between two vertices
    int countPaths(int from, int to);
    
    /// Get all paths as matrix
    PathCollection findAllPaths(int from, int to);

private:
    AdjacencyGraph const& m_graph;
    
    /// DFS backtracking helper
    void dfsExplore(
        int current, 
        int target, 
        std::vector<bool>& visited,
        std::vector<int>& path, 
        PathCollection& allPaths);
};
