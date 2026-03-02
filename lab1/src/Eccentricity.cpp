#include "include/Eccentricity.h"
#include <limits>
#include <algorithm>

namespace graph {

EccentricityCalculator::EccentricityCalculator(const Graph& graph)
    : m_graph_(graph) {}

// Bellman-Ford algorithm - works with negative weights
std::map<int, double> EccentricityCalculator::bellmanFord(int source) const {
    std::map<int, double> dist;
    const double INF = std::numeric_limits<double>::infinity();
    
    // Initialize all distances to infinity, source to 0
    for (int v : m_graph_.vertexIds()) {
        dist[v] = INF;
    }
    dist[source] = 0.0;
    
    int n = m_graph_.size();
    
    // Relax all edges V-1 times
    for (int i = 0; i < n - 1; ++i) {
        bool changed = false;
        
        // For each vertex u
        for (int u : m_graph_.vertexIds()) {
            if (dist[u] == INF) continue;  // Skip unreachable vertices
            
            // For each neighbor v of u
            for (auto const& [v, w] : m_graph_.neighbors(u)) {
                double newDist = dist[u] + w;
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    changed = true;
                }
            }
        }
        
        // Early termination if no changes
        if (!changed) break;
    }
    
    return dist;
}

EccentricityResult EccentricityCalculator::compute() {
    EccentricityResult result;
    const double INF = std::numeric_limits<double>::infinity();
    
    result.radius = INF;
    result.diameter = -INF;
    
    auto vertices = m_graph_.vertexIds();
    
    // Compute eccentricity for each vertex using Bellman-Ford
    for (int v : vertices) {
        auto dists = bellmanFord(v);
        
        // Eccentricity = maximum distance to any reachable vertex
        double ecc = 0.0;
        bool hasReachable = false;
        
        for (auto const& [u, d] : dists) {
            if (u != v && d != INF) {
                ecc = std::max(ecc, d);
                hasReachable = true;
            }
        }
        
        // If no other vertices reachable, eccentricity is 0
        result.eccentricities[v] = hasReachable ? ecc : 0.0;
        result.radius = std::min(result.radius, result.eccentricities[v]);
        result.diameter = std::max(result.diameter, result.eccentricities[v]);
    }
    
    // Find center (vertices with eccentricity = radius)
    for (auto const& [v, ecc] : result.eccentricities) {
        if (std::abs(ecc - result.radius) < 1e-9) {
            result.center.push_back(v);
        }
        if (std::abs(ecc - result.diameter) < 1e-9) {
            result.diametrical.push_back(v);
        }
    }
    
    return result;
}
}
