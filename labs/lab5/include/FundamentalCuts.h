#pragma once
#include "include/Graph.h"
#include <set>
#include <vector>

/// One fundamental cut: associated with one tree edge.
/// Removing the tree edge splits the spanning tree into two components
/// (`sideA` and `sideB`); the cut is every graph edge with exactly one
/// endpoint on each side.  The tree edge itself is always in the cut.
struct FundamentalCut {
    int treeEdgeFrom;            // endpoints of the tree edge that defines this cut
    int treeEdgeTo;
    std::set<int> sideA;         // vertex IDs on one side after removing the tree edge
    std::set<int> sideB;         // vertex IDs on the other side
    std::vector<std::pair<int,int>> edges;  // edges in the cut, normalized so first < second
};

class FundamentalCutSystem {
public:
    /// `originalGraph` is the full (undirected) graph; `spanningTree` must be
    /// a spanning tree built from the same vertex set (e.g. Kruskal's MST).
    FundamentalCutSystem(const AdjacencyGraph& originalGraph,
                         const AdjacencyGraph& spanningTree);

    /// Build all fundamental cuts.  There is exactly one per tree edge, so
    /// the result has size (n - 1) for an n-vertex connected graph.
    std::vector<FundamentalCut> compute() const;

    /// Symmetric difference (XOR) of edge sets:
    ///   present in exactly one of `a`, `b`.
    /// This is the operation that combines cuts in the cut-space basis.
    static std::vector<std::pair<int,int>> symmetricDifference(
        const std::vector<std::pair<int,int>>& a,
        const std::vector<std::pair<int,int>>& b);

private:
    const AdjacencyGraph& m_graph;
    const AdjacencyGraph& m_tree;
};
