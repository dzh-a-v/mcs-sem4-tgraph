#pragma once
#include "include/Graph.h"
#include <memory>
#include <string>
#include <vector>

/// One modification step recorded while making the graph Eulerian.
/// Two reasons we may add an edge:
///   1. The graph (ignoring isolated vertices) was not connected -- we add a
///      bridge between two components so a single cycle can visit everything.
///   2. Two vertices had odd degree -- pairing them up with an extra edge
///      makes both endpoints even, which is required for an Eulerian cycle.
struct EulerianModification {
    int from;
    int to;
    std::string reason;  // human-readable, e.g. "connect components" / "fix odd parity"
};

struct EulerianCycleResult {
    bool wasAlreadyEulerian;                       // true if no edges had to be added
    std::vector<EulerianModification> additions;   // edges added, in the order they were added
    std::vector<int> cycle;                        // vertex sequence v0, v1, ..., vk = v0
    std::unique_ptr<AdjacencyGraph> modifiedGraph; // graph actually used for the cycle (== input if no changes)
    bool success;                                  // false only for pathological inputs (e.g. empty graph)
};

class EulerianCycleBuilder {
public:
    explicit EulerianCycleBuilder(const AdjacencyGraph& graph);

    /// Check the Euler condition (connected + all degrees even on the
    /// "edge-bearing" subgraph), modify the graph if needed (logging every
    /// change), then build an Eulerian cycle with Hierholzer's algorithm.
    EulerianCycleResult compute() const;

private:
    const AdjacencyGraph& m_graph;
};
