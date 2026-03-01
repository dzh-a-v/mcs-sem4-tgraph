#include "include/ShimbellMethod.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace graph {

ShimbellMethod::ShimbellMethod(const Graph& graph)
    : m_graph_(graph), m_vertex_ids_(graph.vertexIds()), m_size_(graph.size()) {}

/// Computes minimum and maximum path distances for paths of exactly k edges.
/// Uses iterative matrix multiplication over (min,+) and (max,+) semirings.
/// Each multiplication step extends paths by one edge.
ShimbellResult ShimbellMethod::compute(int pathLength) {
    if (pathLength <= 0) throw std::invalid_argument("Path length must be positive");

    DistanceMatrix current_min = createAdjacencyMatrix();
    DistanceMatrix current_max = createAdjacencyMatrix();

    if (pathLength == 1) {
        return {current_min, current_max, 1};
    }

    DistanceMatrix base_min = current_min;
    DistanceMatrix base_max = current_max;

    for (int step = 2; step <= pathLength; ++step) {
        current_min = multiplyMin(current_min, base_min);
        current_max = multiplyMax(current_max, base_max);
    }
    return {current_min, current_max, pathLength};
}

/// Builds an adjacency matrix from the graph.
/// Diagonal elements are 0 (distance to self), edges have their weights,
/// and non-adjacent vertex pairs are nullopt (representing infinity).
DistanceMatrix ShimbellMethod::createAdjacencyMatrix() const {
    DistanceMatrix matrix(m_size_, std::vector<std::optional<double>>(m_size_, std::nullopt));
    for (int i = 0; i < m_size_; ++i) matrix[i][i] = 0.0;

    for (auto const& edge : m_graph_.edges()) {
        int from_idx = getIndex(edge.from);
        int to_idx = getIndex(edge.to);
        matrix[from_idx][to_idx] = edge.weight;
        if (!m_graph_.isDirected()) matrix[to_idx][from_idx] = edge.weight;
    }
    return matrix;
}

/// Matrix multiplication over the (min,+) semiring.
/// For each pair (i,j), finds the minimum sum A[i][k] + B[k][j] over all intermediate vertices k.
/// This corresponds to finding the shortest path when combining two path length matrices.
DistanceMatrix ShimbellMethod::multiplyMin(const DistanceMatrix& a, const DistanceMatrix& b) const {
    DistanceMatrix res(m_size_, std::vector<std::optional<double>>(m_size_, std::nullopt));
    for (int i = 0; i < m_size_; ++i) {
        for (int j = 0; j < m_size_; ++j) {
            std::optional<double> best = std::nullopt;
            for (int k = 0; k < m_size_; ++k) {
                if (a[i][k] && b[k][j]) {
                    double val = a[i][k].value() + b[k][j].value();
                    if (!best || val < best.value()) best = val;
                }
            }
            res[i][j] = best;
        }
    }
    return res;
}

/// Matrix multiplication over the (max,+) semiring.
/// For each pair (i,j), finds the maximum sum A[i][k] + B[k][j] over all intermediate vertices k.
/// This corresponds to finding the longest path when combining two path length matrices.
DistanceMatrix ShimbellMethod::multiplyMax(const DistanceMatrix& a, const DistanceMatrix& b) const {
    DistanceMatrix res(m_size_, std::vector<std::optional<double>>(m_size_, std::nullopt));
    for (int i = 0; i < m_size_; ++i) {
        for (int j = 0; j < m_size_; ++j) {
            std::optional<double> best = std::nullopt;
            for (int k = 0; k < m_size_; ++k) {
                if (a[i][k] && b[k][j]) {
                    double val = a[i][k].value() + b[k][j].value();
                    if (!best || val > best.value()) best = val;
                }
            }
            res[i][j] = best;
        }
    }
    return res;
}

/// Converts a vertex ID to its zero-based index in the distance matrices.
/// Uses linear search through the vertex IDs list.
int ShimbellMethod::getIndex(int vertexId) const {
    auto it = std::find(m_vertex_ids_.begin(), m_vertex_ids_.end(), vertexId);
    return static_cast<int>(std::distance(m_vertex_ids_.begin(), it));
}
}