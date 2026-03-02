#include "include/EccentricityBF.h"
#include <limits>
#include <algorithm>

GraphAnalyzerBF::GraphAnalyzerBF(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

std::map<int, double> GraphAnalyzerBF::runBellmanFord(int source) const {
    std::map<int, double> distances;
    constexpr double INF = std::numeric_limits<double>::infinity();
    
    // Initialize: all infinity except source
    for (int v : m_graph.vertexIds()) {
        distances[v] = INF;
    }
    distances[source] = 0.0;
    
    int vertexCount = m_graph.size();
    
    // Bellman-Ford: relax edges V-1 times
    for (int iteration = 0; iteration < vertexCount - 1; ++iteration) {
        bool updated = false;
        
        // For each vertex
        for (int u : m_graph.vertexIds()) {
            if (distances[u] == INF) continue;
            
            // For each neighbor
            for (const auto& [v, weight] : m_graph.neighbors(u)) {
                double newDistance = distances[u] + weight;
                
                if (newDistance < distances[v]) {
                    distances[v] = newDistance;
                    updated = true;
                }
            }
        }
        
        // Early exit if no changes
        if (!updated) break;
    }
    
    return distances;
}

EccentricityData GraphAnalyzerBF::analyze() {
    EccentricityData result;
    constexpr double INF = std::numeric_limits<double>::infinity();
    
    result.radius = INF;
    result.diameter = -INF;
    
    auto vertices = m_graph.vertexIds();
    
    // Compute eccentricity for each vertex
    for (int v : vertices) {
        auto dists = runBellmanFord(v);
        
        // Eccentricity = maximum finite distance
        double ecc = 0.0;
        bool hasReachable = false;
        
        for (const auto& [u, d] : dists) {
            if (u != v && d != INF) {
                ecc = std::max(ecc, d);
                hasReachable = true;
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
