#pragma once
#include "include/Graph.h"
#include <memory>
#include <string>
#include <vector>

/// One modification step recorded while making the graph Eulerian.
/// We may either add an edge or remove one while fixing connectivity/parity.
struct EulerianModification {
    int from;
    int to;
    bool added;          // true = edge added, false = edge removed
    std::string reason;  // human-readable, e.g. "connect components" / "fix odd parity"
};

/// How aggressive the eulerization is allowed to be.
///   NonMultigraphOnly: only add edges between vertices that are not already
///                      connected by an edge.  If the graph cannot be
///                      eulerized this way, the builder fails with
///                      `requiresMultigraph = true` and lets the caller
///                      decide whether to retry in AllowMultigraph mode.
///   AllowMultigraph:   pair odd-degree vertices regardless of whether they
///                      already share an edge -- the resulting graph may be
///                      a multigraph.
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
    bool wasAlreadyEulerian;                       // true if the input already had an Eulerian cycle
    bool wasSemiEulerian;                         // true if the input already had an Eulerian path (but not a cycle)
    std::vector<EulerianModification> additions;   // edge modifications, in the order they were applied
    std::vector<int> traversal;                    // vertex sequence of the Eulerian cycle/path
    std::unique_ptr<AdjacencyGraph> modifiedGraph; // graph actually used for the traversal (== input if no changes)
    bool success;                                  // false only for pathological inputs (e.g. empty graph)
    EulerTraversalKind traversalKind;              // whether `traversal` is a cycle or a path
    /// Set to true only in NonMultigraphOnly mode when eulerization failed
    /// because the only way to fix odd parities would require duplicating
    /// an existing edge (i.e. turning the graph into a multigraph).
    /// When this is true, `success` is false and `traversal` is empty -- the
    /// caller should ask the user whether to retry in AllowMultigraph mode.
    bool requiresMultigraph;
};

class EulerianCycleBuilder {
public:
    explicit EulerianCycleBuilder(const AdjacencyGraph& graph);

    /// Check the Euler condition on the "edge-bearing" subgraph.  If the
    /// graph is already Eulerian, build a cycle.  If it is semi-Eulerian
    /// (exactly two odd-degree vertices), build a path.  Otherwise modify the
    /// graph if needed (logging every change), then build an Eulerian cycle.
    ///
    /// `mode` controls whether the builder is allowed to create parallel
    /// edges.  See EulerizationMode for details.
    EulerianCycleResult compute(EulerizationMode mode = EulerizationMode::NonMultigraphOnly) const;

private:
    const AdjacencyGraph& m_graph;
};
