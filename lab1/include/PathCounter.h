#pragma once
#include "include/Graph.h"
#include <vector>

namespace graph {

/*
 * =============================================================================
 * PATH ENUMERATION VIA BACKTRACKING
 * =============================================================================
 * 
 * Problem: Find ALL simple paths between two vertices in a graph.
 * 
 * A simple path is a path with no repeated vertices.
 * 
 * =============================================================================
 * BACKTRACKING ALGORITHM
 * =============================================================================
 * 
 * The algorithm uses depth-first search (DFS) with backtracking:
 * 
 * 1. Start at the source vertex
 * 2. Mark current vertex as visited
 * 3. Add current vertex to the path
 * 4. If current vertex is the destination:
 *    - Save the complete path
 * 5. Otherwise, for each unvisited neighbor:
 *    - Recursively explore from that neighbor
 * 6. Backtrack: unmark current vertex and remove from path
 * 
 * This explores all possible routes while avoiding cycles.
 * 
 * =============================================================================
 * COMPLEXITY
 * =============================================================================
 * 
 * Time Complexity: O(V! / (V-k)!) in worst case
 *   - Can be exponential for dense graphs
 *   - Number of simple paths can grow factorially
 * 
 * Space Complexity: O(V) for recursion stack + O(P * V) for storing paths
 *   - P = number of paths found
 *   - V = number of vertices
 * 
 * =============================================================================
 * APPLICATIONS
 * =============================================================================
 * 
 * - Network reliability analysis
 * - Finding alternative routes in transportation networks
 * - Counting paths for graph invariants
 * - Verifying connectivity properties
 * =============================================================================
 */

/// Matrix type for storing multiple paths (each path is a vector of vertex IDs)
using PathMatrix = std::vector<std::vector<int>>;

/// Finds and counts all simple paths between two vertices in a graph.
/// Uses backtracking algorithm to enumerate all possible paths.
class PathCounter {
public:
    explicit PathCounter(Graph const& graph);
    int getPathCount(int from, int to);
    PathMatrix getAllPaths(int from, int to);

private:
    Graph const& m_graph_;
    void backtrackAllPaths(int current, int target, std::vector<bool>& visited,
                          std::vector<int>& currentPath, PathMatrix& allPaths);
};
}