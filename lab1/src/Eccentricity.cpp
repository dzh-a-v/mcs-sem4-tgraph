#include "include/Eccentricity.h"
#include <limits>
#include <algorithm>

GraphAnalyzer::GraphAnalyzer(const AdjacencyGraph& graph)
    : m_graph(graph)
    , m_vertexCount(graph.size())
{}

std::map<std::pair<int,int>, double> GraphAnalyzer::computeAllShortestPaths() const {
    std::map<std::pair<int,int>, double> shortestPaths;
    constexpr double INF = std::numeric_limits<double>::infinity();
    
    auto vertexIds = m_graph.vertexIds();
    
    // Initialize all pairs to infinity
    for (int i : vertexIds) {
        for (int j : vertexIds) {
            shortestPaths[{i, j}] = INF;
        }
        shortestPaths[{i, i}] = 0.0;  // Distance to self
    }
    
    // Run Shimbell for k = 1, 2, ..., V-1
    for (int k = 1; k < m_vertexCount; ++k) {
        try {
            KPathCalculator calculator(m_graph);
            auto result = calculator.compute(k);
            
            // Update minimum distances
            for (size_t i = 0; i < vertexIds.size(); ++i) {
                for (size_t j = 0; j < vertexIds.size(); ++j) {
                    if (result.minWeights[i][j].has_value()) {
                        double dist = result.minWeights[i][j].value();
                        double& current = shortestPaths[{vertexIds[i], vertexIds[j]}];
                        if (dist < current) {
                            current = dist;
                        }
                    }
                }
            }
        } catch (const std::exception&) {
            // Skip if Shimbell fails for this k
            continue;
        }
    }
    
    return shortestPaths;
}

EccentricityData GraphAnalyzer::analyze() {
    EccentricityData result;
    constexpr double INF = std::numeric_limits<double>::infinity();
    
    result.radius = INF;
    result.diameter = -INF;
    
    auto vertices = m_graph.vertexIds();
    
    // Compute shortest paths between all pairs
    auto allShortest = computeAllShortestPaths();
    
    // Compute eccentricity for each vertex
    for (int v : vertices) {
        double ecc = 0.0;
        bool hasReachable = false;
        
        for (int u : vertices) {
            if (u != v) {
                double dist = allShortest[{v, u}];
                if (dist != INF) {
                    ecc = std::max(ecc, dist);
                    hasReachable = true;
                }
            }
        }
        
        result.eccentricities[v] = hasReachable ? ecc : 0.0;
        result.radius = std::min(result.radius, result.eccentricities[v]);
        result.diameter = std::max(result.diameter, result.eccentricities[v]);
    }
    
    // Find center and diametrical vertices
    for (const auto& [v, ecc] : result.eccentricities) {
        if (std::abs(ecc - result.radius) < 1e-9) {
            result.centerVertices.push_back(v);
        }
        if (std::abs(ecc - result.diameter) < 1e-9) {
            result.diametricalVertices.push_back(v);
        }
    }
    
    return result;
}
