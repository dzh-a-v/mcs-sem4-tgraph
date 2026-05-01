#pragma once
#include "include/Graph.h"
#include <memory>
#include <vector>

/// Prufer code of a free tree, extended with edge weights.
///
/// Per the lecture material (slide 30), the code has length p - 1 (not p - 2):
/// the textbook variant that drops the last edge is NOT used here.
///
/// At step i of encoding we remove the leaf with the smallest ID and write down:
///   - code[i]    -- the ID of its only neighbor
///   - weights[i] -- the weight of the edge between them
/// This pairing is what makes weight-preserving encode/decode possible.
struct PruferCode {
    std::vector<int> code;        // length = p - 1
    std::vector<double> weights;  // length = p - 1, parallel to `code`
};

class PruferEncoder {
public:
    /// Encode a tree into its Prufer code (with weights preserved).
    /// Precondition: `tree` is a connected acyclic undirected graph (a tree).
    /// If preconditions are violated the result is unspecified.
    static PruferCode encode(const AdjacencyGraph& tree);

    /// Decode a Prufer code (with weights) back into a tree.
    ///
    /// Because the code alone does not record which vertex IDs exist
    /// (e.g. the very last vertex may never appear in the code), the caller
    /// must also pass `vertexIds` -- the full vertex set of the tree to rebuild.
    ///
    /// Returned graph is undirected (free trees are undirected by definition).
    static std::unique_ptr<AdjacencyGraph> decode(
        const PruferCode& pruferCode,
        const std::vector<int>& vertexIds);
};
