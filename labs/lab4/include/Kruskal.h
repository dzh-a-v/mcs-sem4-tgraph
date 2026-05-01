#pragma once
#include "include/Graph.h"
#include <memory>
#include <vector>

/// Result of Kruskal's algorithm
struct KruskalResult {
    std::unique_ptr<AdjacencyGraph> spanningTree;  // MST as an undirected graph
    std::vector<WeightedEdge> chosenEdges;         // edges in the order they were added
    double totalWeight;                            // sum of weights of chosen edges
    bool wasConnected;                             // false if input graph was disconnected
                                                   // (then we return a spanning forest, not a tree)
};

class KruskalMST {
public:
    explicit KruskalMST(const AdjacencyGraph& graph);

    /// Build a minimum spanning tree using Kruskal's algorithm.
    /// The graph is treated as undirected. Edges are sorted by ascending weight;
    /// ties are broken deterministically by (from, to) to make output reproducible.
    KruskalResult buildMST() const;

private:
    const AdjacencyGraph& m_graph;
};
