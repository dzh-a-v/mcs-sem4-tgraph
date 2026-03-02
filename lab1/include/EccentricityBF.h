#pragma once
#include "include/Graph.h"
#include <vector>
#include <optional>
#include <map>

/*
 * =============================================================================
 * ECCENTRICITY ANALYSIS (Bellman-Ford Version)
 * =============================================================================
 * 
 * COMPUTES vertex eccentricities, graph center, and diametrical vertices.
 * 
 * NOTE: This is the Bellman-Ford implementation saved for reference.
 *       Main program uses Shimbell's method instead.
 * 
 * Definitions:
 *   - Eccentricity e(v): max distance from v to any other vertex
 *   - Radius: minimum eccentricity (center vertices have this)
 *   - Diameter: maximum eccentricity (diametrical vertices have this)
 *   - Center: vertices with eccentricity = radius
 * 
 * Algorithm: Bellman-Ford from each vertex (supports negative weights)
 * Complexity: O(V^2 * E)
 * =============================================================================
 */

/// Eccentricity computation result
struct EccentricityData {
    std::map<int, double> eccentricities;  /// e(v) for each vertex
    std::vector<int> centerVertices;       /// Vertices with min eccentricity
    std::vector<int> diametricalVertices;  /// Vertices with max eccentricity
    double radius;                         /// Minimum eccentricity
    double diameter;                       /// Maximum eccentricity
};

/// Eccentricity calculator using Bellman-Ford
class GraphAnalyzerBF {
public:
    /// Initialize with graph
    explicit GraphAnalyzerBF(const AdjacencyGraph& graph);
    
    /// Compute all eccentricities
    EccentricityData analyze();

private:
    const AdjacencyGraph& m_graph;
    
    /// Bellman-Ford shortest paths from source
    std::map<int, double> runBellmanFord(int source) const;
};
