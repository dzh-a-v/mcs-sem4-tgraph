#include "include/FundamentalCuts.h"
#include <algorithm>
#include <unordered_set>

FundamentalCutSystem::FundamentalCutSystem(const AdjacencyGraph& originalGraph,
                                           const AdjacencyGraph& spanningTree)
    : m_graph(originalGraph)
    , m_tree(spanningTree)
{}

namespace {

// Normalize an edge so the smaller endpoint comes first.  This lets us treat
// (u,v) and (v,u) as the same edge when comparing cuts.
std::pair<int,int> normEdge(int u, int v) {
    if (u > v) std::swap(u, v);
    return {u, v};
}

// BFS over the tree starting at `root`, but pretending the edge
// (forbiddenA, forbiddenB) does not exist.  Returns the set of reached
// vertices -- this is one of the two components after removing that edge.
std::set<int> reachableInTreeWithoutEdge(const AdjacencyGraph& tree,
                                         int root,
                                         int forbiddenA,
                                         int forbiddenB) {
    std::set<int> visited;
    std::vector<int> queue{root};
    visited.insert(root);

    size_t head = 0;
    while (head < queue.size()) {
        int u = queue[head++];
        for (const auto& [v, _w] : tree.neighbors(u)) {
            // Skip the edge we are pretending to remove (in either direction).
            const bool isForbidden =
                (u == forbiddenA && v == forbiddenB) ||
                (u == forbiddenB && v == forbiddenA);
            if (isForbidden) continue;
            if (visited.count(v)) continue;
            visited.insert(v);
            queue.push_back(v);
        }
    }
    return visited;
}

}  // namespace

std::vector<FundamentalCut> FundamentalCutSystem::compute() const {
    std::vector<FundamentalCut> cuts;

    // Walk every tree edge and build its fundamental cut.
    // AdjacencyGraph::edges() lists each undirected edge once.
    for (const WeightedEdge& te : m_tree.edges()) {
        FundamentalCut cut;
        cut.treeEdgeFrom = te.from;
        cut.treeEdgeTo = te.to;

        // The two sides of the partition: BFS from te.from in T \ {te},
        // everyone else is on the other side.
        cut.sideA = reachableInTreeWithoutEdge(m_tree, te.from, te.from, te.to);
        for (int v : m_tree.vertexIds()) {
            if (cut.sideA.count(v) == 0) {
                cut.sideB.insert(v);
            }
        }

        // Now scan all ORIGINAL graph edges; an edge is in the cut iff its
        // two endpoints fall on opposite sides of the partition.
        for (const WeightedEdge& e : m_graph.edges()) {
            const bool aHasFrom = cut.sideA.count(e.from) > 0;
            const bool aHasTo   = cut.sideA.count(e.to)   > 0;
            if (aHasFrom != aHasTo) {  // exactly one endpoint in sideA
                cut.edges.push_back(normEdge(e.from, e.to));
            }
        }
        // Sort for deterministic display and to make XOR easier.
        std::sort(cut.edges.begin(), cut.edges.end());
        cuts.push_back(std::move(cut));
    }

    return cuts;
}

std::vector<std::pair<int,int>> FundamentalCutSystem::symmetricDifference(
    const std::vector<std::pair<int,int>>& a,
    const std::vector<std::pair<int,int>>& b)
{
    // Both inputs are sorted (compute() sorts each cut), so the symmetric
    // difference is a single linear merge: keep elements that are in exactly
    // one of the two lists.
    std::vector<std::pair<int,int>> result;
    size_t i = 0;
    size_t j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] == b[j]) {
            // present in both -> drop
            ++i;
            ++j;
        } else if (a[i] < b[j]) {
            result.push_back(a[i++]);
        } else {
            result.push_back(b[j++]);
        }
    }
    while (i < a.size()) result.push_back(a[i++]);
    while (j < b.size()) result.push_back(b[j++]);
    return result;
}
