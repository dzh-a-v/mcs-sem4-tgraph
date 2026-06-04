#pragma once
#include "include/Graph.h"
#include <memory>
#include <string>
#include <vector>

/// One edge add/remove while eulerizing the graph.
struct EulerianModification {
    int from;
    int to;
    bool added;          // true = edge added, false = edge removed
    std::string reason;  // human-readable, e.g. "connect components" / "fix odd parity"
};

/// NonMultigraphOnly: no parallel edges; AllowMultigraph: may duplicate edges;
/// DeleteEdgesOnly: fix parity by removing edges.
enum class EulerizationMode {
    NonMultigraphOnly,
    AllowMultigraph,
    DeleteEdgesOnly
};

enum class EulerTraversalKind {
    None,
    Cycle,
    Path
};

struct EulerianCycleResult {
    bool wasAlreadyEulerian;
    bool wasSemiEulerian;
    std::vector<EulerianModification> additions;
    std::vector<int> traversal;
    std::unique_ptr<AdjacencyGraph> modifiedGraph;
    bool success;
    EulerTraversalKind traversalKind;
    bool requiresMultigraph;  // true if only multigraph eulerization would work
};

class EulerianCycleBuilder {
public:
    explicit EulerianCycleBuilder(const AdjacencyGraph& graph);

    /// Build Eulerian cycle/path; eulerize first if needed (see EulerizationMode).
    EulerianCycleResult compute(
        EulerizationMode mode = EulerizationMode::NonMultigraphOnly,
        bool preferEulerization = false) const;

private:
    const AdjacencyGraph& m_graph;
};
