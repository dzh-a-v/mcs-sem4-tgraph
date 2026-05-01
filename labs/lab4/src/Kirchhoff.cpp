#include "include/Kirchhoff.h"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

KirchhoffCounter::KirchhoffCounter(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

KirchhoffResult KirchhoffCounter::compute() const {
    KirchhoffResult result;
    result.kirchhoffMatrix = buildKirchhoffMatrix();

    const int n = m_graph.size();

    // Trivial cases:
    // n == 0 -- empty graph, no spanning trees
    // n == 1 -- single vertex, one (trivial) spanning tree
    if (n == 0) {
        result.spanningTreeCount = 0;
        return result;
    }
    if (n == 1) {
        result.spanningTreeCount = 1;
        return result;
    }

    // By the matrix-tree theorem: number of spanning trees =
    // any cofactor of the Kirchhoff matrix. We pick the (1,1) cofactor,
    // which is just det(minor obtained by removing row 0 and column 0).
    result.spanningTreeCount = determinantOfFirstMinor(result.kirchhoffMatrix);
    return result;
}

std::vector<std::vector<int>> KirchhoffCounter::buildKirchhoffMatrix() const {
    const int n = m_graph.size();
    std::vector<std::vector<int>> B(n, std::vector<int>(n, 0));

    if (n == 0) {
        return B;
    }

    // Map vertex IDs to row/column indices.
    auto vertexIds = m_graph.vertexIds();
    std::unordered_map<int, int> idToIndex;
    for (int i = 0; i < n; ++i) {
        idToIndex[vertexIds[i]] = i;
    }

    // We treat the graph as undirected: every edge (u, v) contributes
    //   B[u][u] += 1, B[v][v] += 1
    //   B[u][v] -= 1, B[v][u] -= 1
    // AdjacencyGraph::edges() already returns each undirected edge once
    // (filtered by u < v internally), so a single pass over edges() is enough.
    //
    // For directed input, edges() returns each arc once; the user is expected
    // to pass an undirected graph for this algorithm. We still symmetrize the
    // contribution so that the Laplacian is well-defined either way.
    for (const auto& edge : m_graph.edges()) {
        const int i = idToIndex.at(edge.from);
        const int j = idToIndex.at(edge.to);
        if (i == j) {
            continue;  // ignore self-loops, they don't affect tree counts
        }
        B[i][i] += 1;
        B[j][j] += 1;
        B[i][j] -= 1;
        B[j][i] -= 1;
    }

    return B;
}

long long KirchhoffCounter::determinantOfFirstMinor(
    const std::vector<std::vector<int>>& matrix) const
{
    const int n = static_cast<int>(matrix.size());
    if (n <= 1) {
        return 0;  // shouldn't happen, handled by caller, but be safe
    }

    // Build the (n-1) x (n-1) minor in double precision (drop row 0 and column 0).
    const int m = n - 1;
    std::vector<std::vector<double>> M(m, std::vector<double>(m, 0.0));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < m; ++j) {
            M[i][j] = static_cast<double>(matrix[i + 1][j + 1]);
        }
    }

    // Gaussian elimination with partial pivoting.
    // Track sign flips from row swaps so we can recover det's sign at the end.
    double det = 1.0;
    int signFlips = 0;

    for (int col = 0; col < m; ++col) {
        // Find pivot row -- the row at or below `col` with the largest |M[row][col]|.
        int pivot = col;
        double bestAbs = std::fabs(M[col][col]);
        for (int row = col + 1; row < m; ++row) {
            const double currentAbs = std::fabs(M[row][col]);
            if (currentAbs > bestAbs) {
                bestAbs = currentAbs;
                pivot = row;
            }
        }

        // If the pivot is effectively zero, the matrix is singular -> det = 0
        // (this happens, for example, when the graph is disconnected).
        if (bestAbs < 1e-12) {
            return 0;
        }

        // Swap rows if needed and remember the sign flip.
        if (pivot != col) {
            std::swap(M[col], M[pivot]);
            ++signFlips;
        }

        // Multiply running determinant by the pivot.
        det *= M[col][col];

        // Eliminate below: M[row] -= factor * M[col] for row > col.
        const double pivotValue = M[col][col];
        for (int row = col + 1; row < m; ++row) {
            const double factor = M[row][col] / pivotValue;
            if (factor == 0.0) {
                continue;
            }
            // Start from `col` since columns < col are already 0.
            for (int k = col; k < m; ++k) {
                M[row][k] -= factor * M[col][k];
            }
        }
    }

    if (signFlips % 2 != 0) {
        det = -det;
    }

    // The result must be a non-negative integer (number of spanning trees).
    // Round and clamp to handle floating-point noise.
    const double rounded = std::round(det);
    if (rounded < 0.0) {
        return 0;
    }
    return static_cast<long long>(rounded);
}
