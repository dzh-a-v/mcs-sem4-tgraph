#include "include/Eccentricity.h"
#include "include/PathCounter.h"
#include <limits>
#include <algorithm>
#include <functional>

/// Helper function to find all shortest paths between two vertices
static std::vector<std::vector<int>> findAllShortestPaths(
    int start,
    int end,
    const AdjacencyGraph& graph,
    const std::map<std::pair<int,int>, double>& allShortest)
{
    std::vector<std::vector<int>> result;
    constexpr double INF = std::numeric_limits<double>::infinity();

    double targetDist = allShortest.at({start, end});
    if (targetDist == INF || targetDist == 0) {
        return result;
    }

    // Use DFS with pruning based on shortest path distances
    std::vector<int> currentPath;
    currentPath.push_back(start);

    std::function<void(int)> dfs = [&](int current) {
        if (current == end) {
            result.push_back(currentPath);
            return;
        }

        for (const auto& [neighbor, weight] : graph.neighbors(current)) {
            // Check if this edge is on a shortest path
            double distToNeighbor = allShortest.at({start, neighbor});
            double distFromNeighborToEnd = allShortest.at({neighbor, end});

            // Avoid cycles
            if (std::find(currentPath.begin(), currentPath.end(), neighbor) != currentPath.end()) {
                continue;
            }

            // Check if going through this neighbor maintains shortest path property
            if (distToNeighbor != INF && distFromNeighborToEnd != INF) {
                double pathThroughNeighbor = distToNeighbor + distFromNeighborToEnd;
                if (std::abs(pathThroughNeighbor - targetDist) < 1e-9) {
                    // Check if the edge weight matches the distance increment
                    double expectedWeight = distToNeighbor - allShortest.at({start, current});
                    if (std::abs(weight - expectedWeight) < 1e-9 ||
                        std::abs(allShortest.at({start, current}) + weight - distToNeighbor) < 1e-9) {
                        currentPath.push_back(neighbor);
                        dfs(neighbor);
                        currentPath.pop_back();
                    }
                }
            }
        }
    };

    dfs(start);
    return result;
}

GraphAnalyzer::GraphAnalyzer(const AdjacencyGraph& graph)
    : m_graph(graph)
    , m_vertexCount(graph.size())
{}

std::map<std::pair<int,int>, double> GraphAnalyzer::computeAllShortestPaths() const {
    std::map<std::pair<int,int>, double> shortestPaths;
    constexpr double INF = std::numeric_limits<double>::infinity();
    
    auto vertexIds = m_graph.vertexIds();
    
    // Initialize all pairs to infinity so that they are not optimal
    for (int i : vertexIds) {
        for (int j : vertexIds) {
            shortestPaths[{i, j}] = INF;
        }
        shortestPaths[{i, i}] = 0;  // Distance to self eq 0
    }
    
    // Sh For all Ks
    for (int k = 1; k < m_vertexCount; ++k) {
        try {
            KPathCalculator calculator(m_graph);
            auto result = calculator.compute(k);
            
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
            continue; // I'm not sure I'll need it, but it helped me once
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

    auto allShortest = computeAllShortestPaths();

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

        result.eccentricities[v] = hasReachable ? ecc : 0.0; // FtF
        result.radius = std::min(result.radius, result.eccentricities[v]);
        result.diameter = std::max(result.diameter, result.eccentricities[v]);
    }

    for (const auto& [v, ecc] : result.eccentricities) {
        if (std::abs(ecc - result.radius) < 1e-9) {
            result.centerVertices.push_back(v);
        }
        if (std::abs(ecc - result.diameter) < 1e-9) {
            result.diametricalVertices.push_back(v);
        }
    }

    // Find all diametrical paths (shortest paths with length = diameter)
    for (int start : vertices) {
        for (int end : vertices) {
            if (start != end) {
                double dist = allShortest[{start, end}];
                if (std::abs(dist - result.diameter) < 1e-9) {
                    auto paths = findAllShortestPaths(start, end, m_graph, allShortest);
                    result.diametricalPaths.insert(
                        result.diametricalPaths.end(),
                        paths.begin(),
                        paths.end()
                    );
                }
            }
        }
    }

    return result;
}
