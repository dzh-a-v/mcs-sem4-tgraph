#pragma once
#include "include/Graph.h"
#include <vector>
#include <optional>

/*
 * =============================================================================
 * SHIMBEL'S ALGORITHM FOR K-EDGE PATHS
 * =============================================================================
 * 
 * Finds minimum and maximum weight paths with EXACTLY k edges
 * between all pairs of vertices.
 * 
 * Method: Matrix multiplication over semirings
 *   - (min, +) semiring for shortest paths
 *   - (max, +) semiring for longest paths
 * 
 * Key insight: A^k gives optimal k-edge paths
 *   where A is the adjacency matrix
 * 
 * Complexity: O(k * V^3)
 * =============================================================================
 */

/// Distance matrix type (nullopt = infinity/unreachable)
using WeightTable = std::vector<std::vector<std::optional<double>>>;

/// Result container for Shimbell computation
struct ShimbellOutput {
    WeightTable minWeights;     /// Shortest k-edge paths
    WeightTable maxWeights;     /// Longest k-edge paths
    int edgeCount;              /// Number of edges (k)
};

/// Shimbell's method implementation
class KPathCalculator {
public:
    /// Initialize with graph
    explicit KPathCalculator(const AdjacencyGraph& graph);
    
    /// Compute k-edge paths
    ShimbellOutput compute(int edgeCount);

private:
    const AdjacencyGraph& m_graph;
    std::vector<int> m_vertexIds;
    int m_vertexCount;
    
    /// Build adjacency matrix
    WeightTable buildWeightMatrix() const;
    
    /// (min, +) matrix multiplication
    WeightTable multiplyMinPlus(const WeightTable& left, const WeightTable& right) const;
    
    /// (max, +) matrix multiplication  
    WeightTable multiplyMaxPlus(const WeightTable& left, const WeightTable& right) const;
    
    /// Map vertex ID to matrix index
    int mapVertexToIndex(int vertexId) const;
};
