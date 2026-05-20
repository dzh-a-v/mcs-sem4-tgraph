#pragma once
#include "include/Graph.h"
#include "include/MaxMatching.h"

/// Edmonds' blossom algorithm: maximum-cardinality matching in any undirected graph.
///
/// Unlike GreedyMaximalMatching (which produces a matching that is only maximal
/// by inclusion -- no edge can be added without losing independence), this
/// algorithm is guaranteed to find a matching of MAXIMUM size.
///
/// Theoretical foundation:
///   - Berge's theorem (1957): a matching M is maximum iff G has no augmenting
///     path with respect to M.
///   - Edmonds' theorem (1965): an augmenting path in G exists iff one exists
///     in the contracted graph G/B, where B is any blossom.
///
/// The algorithm repeatedly searches for augmenting paths using BFS that
/// handles odd cycles (blossoms) by contracting them. Each augmenting path,
/// when XOR-ed with the current matching, increases the matching size by 1.
///
/// Complexity: O(V^2 * E) in this basic implementation.
/// For our lab-sized graphs (V <= ~20, E <= ~50) this is effectively instant.
///
/// Returns the same MatchingResult struct as GreedyMaximalMatching so it can
/// drop in as a replacement.
class EdmondsMatching {
public:
    explicit EdmondsMatching(const AdjacencyGraph& graph);

    /// Compute a maximum-cardinality matching.
    /// The graph is treated as undirected. Edge weights are ignored:
    /// "maximum" here means "largest by edge count", not "largest by weight".
    MatchingResult compute() const;

private:
    const AdjacencyGraph& m_graph;
};
