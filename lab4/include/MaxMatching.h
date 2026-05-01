#pragma once
#include "include/Graph.h"
#include <vector>

/// Result of greedy maximal matching.
///
/// "Maximal" here is per the lecture slide: a matching whose superset is no
/// longer independent. This is found greedily and is NOT necessarily the
/// largest possible matching (that would be "maximum"); see the slide on
/// Hall's theorem for the bipartite case.
struct MatchingResult {
    std::vector<WeightedEdge> matchingEdges;  // independent edges chosen by greedy pass
    std::vector<int> unmatchedVertices;       // vertices not covered by the matching
    int matchingSize;                         // == matchingEdges.size()
    bool isPerfect;                           // true iff every vertex is matched
};

class GreedyMaximalMatching {
public:
    explicit GreedyMaximalMatching(const AdjacencyGraph& graph);

    /// Build a maximal independent edge set greedily:
    ///   for each edge (u, v) in some order:
    ///     if neither u nor v is matched yet, take this edge
    ///
    /// The graph is treated as undirected. To make output reproducible across
    /// runs on the same input, edges are processed in lexicographic (from, to) order.
    MatchingResult compute() const;

private:
    const AdjacencyGraph& m_graph;
};
