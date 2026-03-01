#pragma once
#include "include/Graph.h"
#include <random>

namespace graph {

/*
 * =============================================================================
 * RANDOM ACYCLIC GRAPH GENERATOR
 * =============================================================================
 * 
 * Generates random acyclic graphs with weighted edges.
 * 
 * =============================================================================
 * ACYCLICITY GUARANTEE
 * =============================================================================
 * 
 * For directed graphs, acyclicity is guaranteed by construction:
 *   - Only edges (u, v) where u < v are allowed
 *   - This creates a topological ordering: 0 → 1 → 2 → ... → n-1
 *   - No back-edges exist, therefore no cycles possible
 * 
 * For undirected graphs:
 *   - Edges are normalized so u < v (canonical representation)
 *   - Prevents duplicate edges between same vertex pair
 * 
 * =============================================================================
 * RAYLEIGH DISTRIBUTION
 * =============================================================================
 * 
 * Edge weights follow the Rayleigh distribution:
 * 
 *   f(x; σ) = (x/σ²) · exp(-x²/(2σ²))  for x ≥ 0
 * 
 * where σ (scale) controls the spread of the distribution.
 * 
 * Properties:
 *   - Mean: σ · √(π/2) ≈ 1.25σ
 *   - Variance: σ² · (2 - π/2) / 2
 *   - Support: [0, ∞)
 * 
 * Generation via inverse transform sampling:
 *   X = σ · √(-2 · ln(1 - U))
 * where U ~ Uniform(0, 1)
 * 
 * Applications:
 *   - Modeling distances in spatial networks
 *   - Signal processing (magnitude of complex Gaussian)
 *   - Wind speed modeling
 * =============================================================================
 */

/// Generates random acyclic graphs with weighted edges.
/// Edge weights follow a Rayleigh distribution.
class Generator {
public:
    /// Generates a random acyclic graph with specified parameters.
    /// @param vertices Number of vertices in the graph
    /// @param edges Number of edges to create
    /// @param directed Whether the graph should be directed
    /// @param scale Scale parameter for Rayleigh distribution (default 1.0)
    /// @return Unique pointer to the generated Graph
    std::unique_ptr<Graph> generateAcyclicGraph(int vertices, int edges, bool directed, double scale = 1.0);
    
private:
    std::mt19937 m_rng{std::random_device{}()};
    
    /// Generates a random weight using Rayleigh distribution.
    /// @param scale Scale parameter of the distribution
    /// @return Random weight value
    double generateRayleighWeight(double scale);
};
}