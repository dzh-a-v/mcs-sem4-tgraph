#include "include/Eccentricity.h"
#include <limits>
#include <algorithm>
#include <functional>
#include <set>
#include <queue>

GraphAnalyzer::GraphAnalyzer(const AdjacencyGraph& graph)
    : m_graph(graph)
    , m_vertexCount(graph.size())
{}

/// Compute shortest paths in terms of edge count using BFS
std::map<std::pair<int,int>, int> GraphAnalyzer::computeEdgeCountPaths() const {
    std::map<std::pair<int,int>, int> distances;
    auto vertices = m_graph.vertexIds();
    
    // For each starting vertex, run BFS to find distances to all others
    for (int start : vertices) {
        std::map<int, int> dist;
        std::queue<int> q;
        
        // Initialize
        for (int v : vertices) {
            dist[v] = -1;  // -1 = unreachable
        }
        dist[start] = 0;
        q.push(start);
        
        // BFS
        while (!q.empty()) {
            int current = q.front();
            q.pop();
            
            for (const auto& [neighbor, weight] : m_graph.neighbors(current)) {
                if (dist[neighbor] == -1) {  // Not visited
                    dist[neighbor] = dist[current] + 1;
                    q.push(neighbor);
                }
            }
        }
        
        // Store distances
        for (int end : vertices) {
            if (dist[end] >= 0) {
                distances[{start, end}] = dist[end];
            } else {
                distances[{start, end}] = -1;  // Unreachable
            }
        }
    }
    
    return distances;
}

/// Find all paths with exactly k edges between two vertices using DFS
std::vector<std::vector<int>> GraphAnalyzer::findAllPathsWithExactEdges(
    int start,
    int end,
    int edgeCount,
    const std::map<std::pair<int,int>, int>& edgeCounts) const
{
    std::vector<std::vector<int>> result;
    
    // Check if path exists with exactly edgeCount edges
    if (edgeCounts.at({start, end}) != edgeCount) {
        return result;  // No such path
    }
    
    // DFS to find all paths with exactly edgeCount edges
    std::vector<int> currentPath;
    currentPath.push_back(start);
    
    std::function<void(int, int)> dfs = [&](int current, int edgesUsed) {
        if (edgesUsed == edgeCount) {
            if (current == end) {
                result.push_back(currentPath);
            }
            return;
        }
        
        // Remaining edges needed
        int remaining = edgeCount - edgesUsed;
        
        for (const auto& [neighbor, weight] : m_graph.neighbors(current)) {
            // Avoid cycles
            if (std::find(currentPath.begin(), currentPath.end(), neighbor) != currentPath.end()) {
                continue;
            }
            
            // Check if we can reach end with exactly remaining edges from neighbor
            int distToEnd = edgeCounts.at({neighbor, end});
            if (distToEnd >= 0 && distToEnd == remaining - 1) {
                currentPath.push_back(neighbor);
                dfs(neighbor, edgesUsed + 1);
                currentPath.pop_back();
            }
        }
    };
    
    dfs(start, 0);
    return result;
}

EccentricityData GraphAnalyzer::analyze() {
    EccentricityData result;
    result.radius = std::numeric_limits<int>::max();
    result.diameter = -1;

    auto vertices = m_graph.vertexIds();
    auto edgeCounts = computeEdgeCountPaths();

    // Compute eccentricities (maximum edge distance for each vertex)
    for (int v : vertices) {
        int ecc = -1;  // -1 = unreachable to all
        bool hasReachable = false;

        for (int u : vertices) {
            if (u != v) {
                int dist = edgeCounts[{v, u}];
                if (dist >= 0) {
                    ecc = std::max(ecc, dist);
                    hasReachable = true;
                }
            }
        }

        // If no reachable vertices (end/sink vertex), set eccentricity to 0
        // TO REVERT TO INFINITE ECCENTRICITY FOR END VERTICES:
        // Change: result.eccentricities[v] = hasReachable ? ecc : 0;
        // To:     result.eccentricities[v] = hasReachable ? ecc : -1;
        // And update the radius/diameter check to: if (result.eccentricities[v] >= 0)
        result.eccentricities[v] = hasReachable ? ecc : 0;
        
        // Only consider vertices with non-negative eccentricity for radius/diameter
        if (result.eccentricities[v] >= 0) {
            result.radius = std::min(result.radius, result.eccentricities[v]);
            result.diameter = std::max(result.diameter, result.eccentricities[v]);
        }
    }

    // Find center and diametrical vertices
    for (const auto& [v, ecc] : result.eccentricities) {
        if (ecc == result.radius) {
            result.centerVertices.push_back(v);
        }
        if (ecc == result.diameter) {
            result.diametricalVertices.push_back(v);
        }
    }

    // Find all diametrical pairs (unique unordered pairs at diameter edge distance)
    std::set<std::pair<int, int>> diametricalPairs;
    
    for (int start : vertices) {
        for (int end : vertices) {
            if (start != end) {
                int dist = edgeCounts[{start, end}];
                if (dist == result.diameter) {
                    // Normalize pair: store as (min, max)
                    int first = std::min(start, end);
                    int second = std::max(start, end);
                    diametricalPairs.insert({first, second});
                }
            }
        }
    }
    
    // Find all diametrical paths for each unique pair
    for (const auto& [v1, v2] : diametricalPairs) {
        auto paths = findAllPathsWithExactEdges(v1, v2, result.diameter, edgeCounts);
        if (!paths.empty()) {
            result.diametricalPathsByPair[{v1, v2}] = paths;
        }
    }

    return result;
}
