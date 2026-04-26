#include "include/ShimbellMethod.h"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <iomanip>

KPathCalculator::KPathCalculator(const AdjacencyGraph& graph)
    : m_graph(graph)
    , m_vertexIds(graph.vertexIds())
    , m_vertexCount(graph.size())
{}

ShimbellOutput KPathCalculator::compute(int edgeCount) {
    if (edgeCount < 0) {
        throw std::invalid_argument("Edge count must be non-negative");
    }

    if (edgeCount == 0) {
        WeightTable zeroEdgeMatrix(
            m_vertexCount,
            std::vector<std::optional<double>>(m_vertexCount, std::nullopt)
        );

        for (int i = 0; i < m_vertexCount; ++i) {
            zeroEdgeMatrix[i][i] = 0.0;
        }

        return {zeroEdgeMatrix, zeroEdgeMatrix, 0};
    }
    
    // Step 1: Build the base matrix for 1-edge paths (direct connections)
    WeightTable baseMatrix = buildWeightMatrix();
    
    // If we only want 1-edge paths, just return the base matrix with diagonal set to 0
    // (diagonal = 0 means no cost for staying at the same vertex, but we don't count it as a path)
    if (edgeCount == 1) {
        for (int i = 0; i < m_vertexCount; ++i) {
            baseMatrix[i][i] = 0.0;
        }
        return {baseMatrix, baseMatrix, 1};
    }
    
    // Step 2: For more edges (k > 1), we "raise the matrix to the power k" using special multiplication
    // Think of it like: start with 1-edge paths, then multiply by the base to get 2-edge paths,
    // multiply again for 3-edge paths, and so on, up to k edges.
    // We use min-plus multiplication for shortest paths (smallest total weight) and
    // max-plus for longest paths (biggest total weight).
    // This is like matrix exponentiation, but in a "tropical" way for graphs!
    WeightTable minMatrix = baseMatrix;  // Starts as A^1
    WeightTable maxMatrix = baseMatrix;  // Starts as A^1
    
    // Loop from 2 to k: each time, multiply the current matrix by the base matrix
    // This builds up paths with exactly 'step' edges
    for (int step = 2; step <= edgeCount; ++step) {
        // Right now, minMatrix has paths with exactly (step-1) edges.
        // We multiply it by baseMatrix (1-edge paths) to get paths with exactly 'step' edges.
        // For example: 2-edge paths = (1-edge paths) combined with (1-edge paths)
        minMatrix = multiplyMinPlus(minMatrix, baseMatrix);
        maxMatrix = multiplyMaxPlus(maxMatrix, baseMatrix);
    }
    
    // Step 3: Set diagonal to 0 in the final result (no self-cost for the paths we found)
    for (int i = 0; i < m_vertexCount; ++i) {
        minMatrix[i][i] = 0.0;
        maxMatrix[i][i] = 0.0;
    }
    
    return {minMatrix, maxMatrix, edgeCount};
}

WeightTable KPathCalculator::buildWeightMatrix() const {
    // Initialize with nullopt (no path)
    WeightTable matrix(
        m_vertexCount, 
        std::vector<std::optional<double>>(m_vertexCount, std::nullopt)
    );
    
    // Diagonal = nullopt (no self-loops for exact k-edge paths)
    // This ensures we only find paths with EXACTLY k edges, not reusing old results
    
    // Fill edge weights (direct edges only)
    for (const auto& edge : m_graph.edges()) {
        int row = mapVertexToIndex(edge.from);
        int col = mapVertexToIndex(edge.to);
        matrix[row][col] = edge.weight;
        
        // Undirected: symmetric
        if (!m_graph.isDirected()) {
            matrix[col][row] = edge.weight;
        }
    }
    
    return matrix;
}

WeightTable KPathCalculator::multiplyMinPlus(
    const WeightTable& left, 
    const WeightTable& right) const 
{
    WeightTable result(
        m_vertexCount,
        std::vector<std::optional<double>>(m_vertexCount, std::nullopt)
    );
    
    // For each cell (i, j)
    for (int i = 0; i < m_vertexCount; ++i) {
        for (int j = 0; j < m_vertexCount; ++j) {
            std::optional<double> minimum = std::nullopt;
            
            // Try all intermediate vertices k
            for (int k = 0; k < m_vertexCount; ++k) {
                if (left[i][k].has_value() && right[k][j].has_value()) {
                    // Perform min-plus multiplication: sum the weights and track the minimum
                    double sum = left[i][k].value() + right[k][j].value();
                    
                    if (!minimum.has_value() || sum < minimum.value()) {
                        minimum = sum;
                    }
                }
            }
            
            result[i][j] = minimum;
        }
    }
    
    return result;
}

WeightTable KPathCalculator::multiplyMaxPlus(
    const WeightTable& left, 
    const WeightTable& right) const 
{
    WeightTable result(
        m_vertexCount,
        std::vector<std::optional<double>>(m_vertexCount, std::nullopt)
    );
    
    // For each cell (i, j)
    for (int i = 0; i < m_vertexCount; ++i) {
        for (int j = 0; j < m_vertexCount; ++j) {
            std::optional<double> maximum = std::nullopt;
            
            // Try all intermediate vertices k
            for (int k = 0; k < m_vertexCount; ++k) {
                if (left[i][k].has_value() && right[k][j].has_value()) {
                    // Perform max-plus multiplication: sum the weights and track the maximum
                    double sum = left[i][k].value() + right[k][j].value();
                    
                    if (!maximum.has_value() || sum > maximum.value()) {
                        maximum = sum;
                    }
                }
            }
            
            result[i][j] = maximum;
        }
    }
    
    return result;
}

int KPathCalculator::mapVertexToIndex(int vertexId) const {
    auto iter = std::find(m_vertexIds.begin(), m_vertexIds.end(), vertexId);
    return static_cast<int>(std::distance(m_vertexIds.begin(), iter));
}

void printWeightTable(std::ostream& out, const WeightTable& table, 
                      const std::vector<int>& vertexIds) {
    const size_t n = table.size();
    if (n == 0) return;

    // Print header row
    out << "      ";
    for (int v : vertexIds) {
        out << std::setw(10) << v;
    }
    out << "\n";

    // Print separator
    out << "      ";
    for (size_t i = 0; i < n; ++i) {
        out << "----------";
    }
    out << "\n";

    // Print matrix rows
    for (size_t i = 0; i < n; ++i) {
        out << std::setw(5) << vertexIds[i] << " |";
        for (size_t j = 0; j < n; ++j) {
            if (table[i][j].has_value()) {
                out << std::setw(10) << table[i][j].value();
            } else {
                out << std::setw(10) << "i";  // i = infinity (no path)
            }
        }
        out << "\n";
    }
}

void printAdjacencyMatrix(std::ostream& out, const AdjacencyMatrix& matrix, 
                          const std::vector<int>& vertexIds) {
    const size_t n = matrix.size();
    if (n == 0) return;

    // Print header row
    out << "      ";
    for (int v : vertexIds) {
        out << std::setw(10) << v;
    }
    out << "\n";

    // Print separator
    out << "      ";
    for (size_t i = 0; i < n; ++i) {
        out << "----------";
    }
    out << "\n";

    // Print matrix rows
    for (size_t i = 0; i < n; ++i) {
        out << std::setw(5) << vertexIds[i] << " |";
        for (size_t j = 0; j < n; ++j) {
            // true = 1 (edge exists), false = 0 (no edge)
            out << std::setw(10) << (matrix[i][j] ? 1 : 0);
        }
        out << "\n";
    }
}
