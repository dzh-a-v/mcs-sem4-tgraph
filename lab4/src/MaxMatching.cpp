#include "include/MaxMatching.h"
#include <algorithm>
#include <unordered_set>

GreedyMaximalMatching::GreedyMaximalMatching(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

MatchingResult GreedyMaximalMatching::compute() const {
    MatchingResult result;
    result.matchingSize = 0;
    result.isPerfect = false;

    const int n = m_graph.size();
    if (n == 0) {
        return result;
    }

    // Pull all edges. AdjacencyGraph::edges() yields each undirected edge once.
    std::vector<WeightedEdge> allEdges = m_graph.edges();

    // Sort lexicographically by (from, to) so the same input always produces
    // the same matching. The greedy algorithm is correct under any order, but
    // a deterministic order is important for reproducible labs.
    std::sort(allEdges.begin(), allEdges.end(),
        [](const WeightedEdge& a, const WeightedEdge& b) {
            if (a.from != b.from) return a.from < b.from;
            return a.to < b.to;
        });

    // matched: set of vertex IDs already covered by the matching.
    // We use unordered_set keyed by vertex ID directly -- IDs may not be
    // 0..n-1 in general, so we don't assume dense indexing here.
    std::unordered_set<int> matched;
    matched.reserve(static_cast<size_t>(n));

    // Greedy pass: take an edge iff both endpoints are still free.
    // After this loop, no edge can be added without sharing an endpoint
    // with one already chosen -> the result is maximal by inclusion.
    for (const WeightedEdge& edge : allEdges) {
        if (edge.from == edge.to) {
            continue;  // self-loop: not a valid matching edge, skip
        }
        if (matched.count(edge.from) == 0 && matched.count(edge.to) == 0) {
            result.matchingEdges.push_back(edge);
            matched.insert(edge.from);
            matched.insert(edge.to);
        }
    }

    result.matchingSize = static_cast<int>(result.matchingEdges.size());

    // Collect vertices not covered. If empty, the matching is perfect.
    for (int v : m_graph.vertexIds()) {
        if (matched.count(v) == 0) {
            result.unmatchedVertices.push_back(v);
        }
    }
    result.isPerfect = result.unmatchedVertices.empty();

    return result;
}
