#pragma once
#include "include/Graph.h"
#include <vector>

/// One fundamental cycle: associated with one non-tree edge ("chord").
/// In a spanning tree T of G, every edge of G that is NOT in T forms a
/// unique cycle when added to T -- the chord plus the unique tree path
/// connecting its two endpoints.  That cycle is the fundamental cycle
/// of the chord.
struct FundamentalCycle {
    int chordFrom;                            // endpoints of the chord that defines this cycle
    int chordTo;
    std::vector<int> vertexSequence;          // cycle as v0, v1, ..., vk = v0 (for display)
    std::vector<std::pair<int,int>> edges;    // edges in the cycle, normalized so first < second; sorted
};

class FundamentalCycleSystem {
public:
    /// `originalGraph` is the full (undirected) graph; `spanningTree` must be
    /// a spanning tree built from the same vertex set (e.g. Kruskal's MST).
    FundamentalCycleSystem(const AdjacencyGraph& originalGraph,
                           const AdjacencyGraph& spanningTree);

    /// Build all fundamental cycles.  There is exactly one per chord, so
    /// the result has size m - n + 1 for a connected n-vertex, m-edge graph.
    std::vector<FundamentalCycle> compute() const;

    /// Symmetric difference (XOR) of edge sets:
    ///   present in exactly one of `a`, `b`.
    /// XOR-ing fundamental cycles in the cycle space gives every cycle of G
    /// (or a disjoint union of cycles, i.e. an "even subgraph").
    /// Both inputs are expected to be sorted (compute() returns sorted edge
    /// lists), and the output is sorted too.
    static std::vector<std::pair<int,int>> symmetricDifference(
        const std::vector<std::pair<int,int>>& a,
        const std::vector<std::pair<int,int>>& b);

private:
    const AdjacencyGraph& m_graph;
    const AdjacencyGraph& m_tree;
};