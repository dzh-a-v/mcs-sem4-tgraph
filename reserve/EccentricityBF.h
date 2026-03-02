#pragma once
#include "include/Graph.h"
#include <vector>
#include <optional>
#include <map>

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
