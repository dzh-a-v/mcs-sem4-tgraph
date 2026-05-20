#pragma once
#include "include/Graph.h"
#include "include/MaxMatching.h"

/// Edmonds blossom algorithm for maximum matching in an undirected graph.
/// Finds the largest matching by number of edges, not by total weight.
class EdmondsMatching {
public:
    explicit EdmondsMatching(const AdjacencyGraph& graph);

    MatchingResult compute() const;

private:
    const AdjacencyGraph& m_graph;
};
