#pragma once
#include "include/Graph.h"
#include <vector>
#include <optional>

namespace graph {

/*
 * =============================================================================
 * SHIMBELL'S METHOD FOR k-EDGE PATHS
 * =============================================================================
 * 
 * Problem: Find shortest and longest paths containing EXACTLY k edges
 * between all pairs of vertices in a weighted graph.
 * 
 * This differs from classic shortest path algorithms (Dijkstra, Bellman-Ford)
 * which find paths with ANY number of edges.
 * 
 * =============================================================================
 * MATRIX MULTIPLICATION OVER SEMIRINGS
 * =============================================================================
 * 
 * The algorithm uses matrix multiplication over algebraic semirings:
 * 
 * (min,+) Semiring (Tropical Semiring):
 *   - Addition:  a ⊕ b = min(a, b)
 *   - Multiplication: a ⊗ b = a + b
 *   - Identity for ⊕: ∞ (infinity)
 *   - Identity for ⊗: 0
 * 
 * (max,+) Semiring:
 *   - Addition:  a ⊕ b = max(a, b)
 *   - Multiplication: a ⊗ b = a + b
 *   - Identity for ⊕: -∞ (negative infinity)
 *   - Identity for ⊗: 0
 * 
 * =============================================================================
 * ALGORITHM
 * =============================================================================
 * 
 * Let A be the adjacency matrix where:
 *   - A[i][j] = weight(i,j) if edge exists
 *   - A[i][j] = ∞ (nullopt) otherwise
 *   - A[i][i] = 0 (distance to self)
 * 
 * Matrix multiplication C = A ⊗ B:
 *   C[i][j] = min/max over all k of (A[i][k] + B[k][j])
 * 
 * Key insight: A^k[i][j] gives the optimal path weight from i to j
 * using exactly k edges.
 * 
 * Proof by induction:
 *   - Base: A^1 = A (paths of length 1 = direct edges)
 *   - Step: A^k = A^(k-1) ⊗ A extends paths by one edge
 * 
 * Time Complexity: O(k · |V|³)
 * Space Complexity: O(|V|²)
 * =============================================================================
 */

/// Matrix type for storing distances between all vertex pairs.
/// nullopt represents no path (infinity).
using DistanceMatrix = std::vector<std::vector<std::optional<double>>>;

/// Result structure containing minimum and maximum path distances
/// for paths of a specific length.
struct ShimbellResult {
    DistanceMatrix min_distances;  /// Minimum distances between all pairs
    DistanceMatrix max_distances;  /// Maximum distances between all pairs
    int path_length;               /// Number of edges in the paths
};

/// Implements Shimbell's method for finding shortest and longest paths
/// of exactly k edges between all pairs of vertices.
/// Uses matrix multiplication approach over (min,+) and (max,+) semirings.
class ShimbellMethod {
public:
    /// Constructs ShimbellMethod for the given graph.
    /// @param graph Reference to the graph to analyze
    explicit ShimbellMethod(const Graph& graph);

    /// Computes minimum and maximum distances for paths of exactly k edges.
    /// @param pathLength Number of edges in the paths (must be > 0)
    /// @return ShimbellResult containing distance matrices
    /// @throws std::invalid_argument if pathLength <= 0
    [[nodiscard]] ShimbellResult compute(int pathLength);

private:
    const Graph& m_graph_;
    std::vector<int> m_vertex_ids_;  /// Mapping from index to vertex ID
    int m_size_;                     /// Number of vertices

    /// Creates adjacency matrix representation of the graph.
    /// Diagonal elements are 0, edges have their weights, non-edges are nullopt.
    DistanceMatrix createAdjacencyMatrix() const;

    /// Matrix multiplication over (min,+) semiring.
    /// C[i][j] = min(A[i][k] + B[k][j]) for all k
    DistanceMatrix multiplyMin(const DistanceMatrix& a, const DistanceMatrix& b) const;

    /// Matrix multiplication over (max,+) semiring.
    /// C[i][j] = max(A[i][k] + B[k][j]) for all k
    DistanceMatrix multiplyMax(const DistanceMatrix& a, const DistanceMatrix& b) const;

    /// Converts vertex ID to matrix index.
    /// @param vertexId The vertex ID to convert
    /// @return Zero-based index in the distance matrices
    int getIndex(int vertexId) const;
};
}