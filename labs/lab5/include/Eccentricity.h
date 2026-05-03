#pragma once
#include "include/Graph.h"
#include "include/BFS.h"
#include <vector>
#include <optional>
#include <map>

/// Eccentricity computation result
struct EccentricityData {
    std::map<int, int> eccentricities;  // in number of edges
    std::vector<int> centerVertices;
    std::vector<int> diametricalVertices;
    // Maps unordered vertex pairs to their diametrical paths
    std::map<std::pair<int, int>, std::vector<std::vector<int>>> diametricalPathsByPair;
    int radius;    // minimum eccentricity (in edges)
    int diameter;  // maximum eccentricity (in edges)
};


class GraphAnalyzer {
public:
    explicit GraphAnalyzer(const AdjacencyGraph& graph);
    
    EccentricityData analyze();

private:
    const AdjacencyGraph& m_graph;
    int m_vertexCount;
    
    // Compute shortest paths in terms of edge count
    std::map<std::pair<int,int>, int> computeEdgeCountPaths() const;
    
    // Find all shortest paths with exactly k edges between two vertices
    std::vector<std::vector<int>> findAllPathsWithExactEdges(
        int start, int end, int edgeCount,
        const std::map<std::pair<int,int>, int>& edgeCounts) const;
};
