#pragma once
#include "include/Graph.h"
#include <vector>

// Matrix storing multiple paths (each row = one path)
using PathCollection = std::vector<std::vector<int>>;

class SimplePathFinder {
public:
    explicit SimplePathFinder(AdjacencyGraph const& graph);
    int countPaths(int from, int to);
    PathCollection findAllPaths(int from, int to);

private:
    AdjacencyGraph const& m_graph;
    
    // backtrack
    void dfsExplore(
        int current, 
        int target, 
        std::vector<bool>& visited,
        std::vector<int>& path, 
        PathCollection& allPaths);
};
