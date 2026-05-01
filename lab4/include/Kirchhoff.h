#pragma once
#include "include/Graph.h"
#include <vector>

/// Result of Kirchhoff's matrix-tree theorem
struct KirchhoffResult {
    std::vector<std::vector<int>> kirchhoffMatrix;  // B(G) -- the Kirchhoff (Laplacian) matrix
    long long spanningTreeCount;                    // number of spanning trees
};

class KirchhoffCounter {
public:
    explicit KirchhoffCounter(const AdjacencyGraph& graph);

    /// Build Kirchhoff matrix and count spanning trees via the matrix-tree theorem.
    /// The graph is treated as undirected; weights are ignored (we count trees, not weight them).
    KirchhoffResult compute() const;

private:
    const AdjacencyGraph& m_graph;

    /// Build the Kirchhoff (Laplacian) matrix B = D - A,
    /// where D is the diagonal degree matrix and A is the adjacency matrix.
    std::vector<std::vector<int>> buildKirchhoffMatrix() const;

    /// Compute determinant of the (n-1) x (n-1) minor obtained by deleting
    /// the first row and the first column. By the matrix-tree theorem, this
    /// determinant equals the number of spanning trees.
    /// Uses Gaussian elimination with partial pivoting for numerical stability,
    /// then rounds the result (it must be an integer for an integer Laplacian).
    long long determinantOfFirstMinor(const std::vector<std::vector<int>>& matrix) const;
};
