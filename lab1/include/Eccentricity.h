#pragma once
#include "include/Graph.h"
#include "include/ShimbellMethod.h"
#include <vector>
#include <optional>
#include <map>

/// Eccentricity computation result
struct EccentricityData {
    std::map<int, double> eccentricities; 
    std::vector<int> centerVertices;
    std::vector<int> diametricalVertices;
    double radius;
    double diameter;
};


class GraphAnalyzer {
public:
    explicit GraphAnalyzer(const AdjacencyGraph& graph);
    
    EccentricityData analyze();

private:
    const AdjacencyGraph& m_graph;
    int m_vertexCount;
    
    std::map<std::pair<int,int>, double> computeAllShortestPaths() const;
};
