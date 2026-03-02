#pragma once
#include "include/Graph.h"
#include <vector>
#include <optional>
#include <map>

namespace graph {

/*
 * =============================================================================
 * GRAPH ECCENTRICITY ANALYSIS
 * =============================================================================
 * 
 * Computes eccentricities, graph center, and diametrical vertices.
 * 
 * Key Definitions:
 *   - Eccentricity e(v): maximum distance from v to any other vertex
 *   - Radius: minimum eccentricity among all vertices
 *   - Diameter: maximum eccentricity among all vertices
 *   - Center: set of vertices with eccentricity = radius
 *   - Diametrical vertices: vertices with eccentricity = diameter
 * 
 * Algorithm:
 *   - Uses Bellman-Ford for shortest paths (supports negative weights)
 *   - Time Complexity: O(V^2 × E) - runs Bellman-Ford from each vertex
 *   - Space Complexity: O(V^2) for distance matrix
 * =============================================================================
 */

struct EccentricityResult {
    std::map<int, double> eccentricities;  /// Eccentricity for each vertex
    std::vector<int> center;               /// Vertices with minimum eccentricity
    std::vector<int> diametrical;          /// Vertices with maximum eccentricity
    double radius;                         /// Minimum eccentricity
    double diameter;                       /// Maximum eccentricity
};

class EccentricityCalculator {
public:
    explicit EccentricityCalculator(const Graph& graph);
    
    /// Computes eccentricities and finds center/diametrical vertices
    EccentricityResult compute();

private:
    const Graph& m_graph_;
    
    /// Computes shortest paths from source to all other vertices
    std::map<int, double> dijkstra(int source) const;
};
}
