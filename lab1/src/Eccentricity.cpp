#include "include/Eccentricity.h"
#include <queue>
#include <limits>
#include <algorithm>

namespace graph {

EccentricityCalculator::EccentricityCalculator(const Graph& graph)
    : m_graph_(graph) {}

std::map<int, double> EccentricityCalculator::dijkstra(int source) const {
    std::map<int, double> dist;
    const double INF = std::numeric_limits<double>::infinity();
    
    // Initialize distances
    for (int v : m_graph_.vertexIds()) {
        dist[v] = INF;
    }
    dist[source] = 0.0;
    
    // Priority queue: (distance, vertex)
    using P = std::pair<double, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.push({0.0, source});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        
        if (d > dist[u]) continue;
        
        for (auto const& [v, w] : m_graph_.neighbors(u)) {
            double newDist = dist[u] + w;
            if (newDist < dist[v]) {
                dist[v] = newDist;
                pq.push({newDist, v});
            }
        }
    }
    
    return dist;
}

EccentricityResult EccentricityCalculator::compute() {
    EccentricityResult result;
    const double INF = std::numeric_limits<double>::infinity();
    
    result.radius = INF;
    result.diameter = -INF;
    
    auto vertices = m_graph_.vertexIds();
    
    // Compute eccentricity for each vertex
    for (int v : vertices) {
        auto dists = dijkstra(v);
        
        // Eccentricity = maximum distance to any reachable vertex
        double ecc = 0.0;
        for (auto const& [u, d] : dists) {
            if (u != v && d != INF) {
                ecc = std::max(ecc, d);
            }
        }
        
        result.eccentricities[v] = ecc;
        result.radius = std::min(result.radius, ecc);
        result.diameter = std::max(result.diameter, ecc);
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
