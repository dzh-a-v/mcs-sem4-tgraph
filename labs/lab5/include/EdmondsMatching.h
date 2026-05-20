#pragma once
#include "include/Graph.h"
#include "include/MaxMatching.h"

/// Edmonds' blossom algorithm: maximum-cardinality matching in any undirected graph.
///
/// The algorithm repeatedly searches for augmenting paths using BFS that
/// handles odd cycles (blossoms) by contracting them. Each augmenting path,
/// when XOR-ed with the current matching, increases the matching size by 1.
///
/// Complexity: O(V^2 * E) in this basic implementation.
/// For our lab-sized graphs this is effectively instant.
///
/// Returns the same MatchingResult struct as GreedyMaximalMatching so it can
/// drop in as a replacement.
class EdmondsMatching {
public:
    explicit EdmondsMatching(const AdjacencyGraph& graph);

    MatchingResult compute() const;

private:
    const AdjacencyGraph& m_graph;
};
