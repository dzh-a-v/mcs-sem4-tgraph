#pragma once
#include "include/Graph.h"
#include "include/BFS.h"
#include "include/Dijkstra.h"

/// Comparison result between algorithms
struct AlgorithmComparison {
    int bfsIterations;
    int dijkstraIterations;
    double speedupFactor;  // How many times faster BFS is
};

class AlgorithmComparator {
public:
    explicit AlgorithmComparator(const AdjacencyGraph& graph);
    
    /// Compare BFS and Dijkstra on the same graph
    /// Returns iteration counts and speedup factor
    AlgorithmComparison compare(int startVertex);

private:
    const AdjacencyGraph& m_graph;
};
