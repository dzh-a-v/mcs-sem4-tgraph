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
    if (edgeCount <= 0) {
        throw std::invalid_argument("Edge count must be positive");
    }
    
    // Initialize with weight matrix (k=1)
    WeightTable baseMatrix = buildWeightMatrix();
    
    // Base case: k=1
    if (edgeCount == 1) {
        // Add diagonal = 0 for k=1 results
        for (int i = 0; i < m_vertexCount; ++i) {
            baseMatrix[i][i] = 0.0;
        }
        return {baseMatrix, baseMatrix, 1};
    }
    
    // For k > 1, compute A^k by multiplying A^(k-1) * A^1 step by step
    // Each iteration produces paths with EXACTLY that many edges
    WeightTable minMatrix = baseMatrix;
    WeightTable maxMatrix = baseMatrix;
    
    for (int step = 2; step <= edgeCount; ++step) {
        // minMatrix currently contains paths with exactly (step-1) edges
        // Multiply by baseMatrix (which has paths with exactly 1 edge)
        // Result has paths with exactly step edges
        minMatrix = multiplyMinPlus(minMatrix, baseMatrix);
        maxMatrix = multiplyMaxPlus(maxMatrix, baseMatrix);
    }
    
    // Add diagonal = 0 for result
    for (int i = 0; i < m_vertexCount; ++i) {
        minMatrix[i][i] = 0.0;
        maxMatrix[i][i] = 0.0;
    }
    
    return {minMatrix, maxMatrix, edgeCount};
}

WeightTable KPathCalculator::buildWeightMatrix() const {
    const double INF_PLACEHOLDER = std::numeric_limits<double>::infinity();
    
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
