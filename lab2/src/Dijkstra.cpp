#include "include/Dijkstra.h"
#include <limits>
#include <algorithm>
#include <set>

DijkstraAlgorithm::DijkstraAlgorithm(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

DijkstraResult DijkstraAlgorithm::findShortestPaths(int startVertex, std::optional<int> targetVertex) {
    DijkstraResult result;
    result.iterations = 0;
    result.targetDistance = std::numeric_limits<double>::infinity();
    
    if (!m_graph.hasVertex(startVertex)) {
        return result;
    }
    
    constexpr double INF = std::numeric_limits<double>::infinity();
    
    auto vertexIds = m_graph.vertexIds();
    int vertexCount = m_graph.size();
    
    // Initialize distances to infinity
    for (int v : vertexIds) {
        result.distances[v] = INF;
    }
    result.distances[startVertex] = 0.0;
    
    // Set of unvisited vertices (ordered by distance)
    std::set<std::pair<double, int>> unvisited;
    unvisited.insert({0.0, startVertex});
    result.iterations++;  // Count: inserting start vertex

    // Track which vertices have been finalized
    std::vector<bool> finalized(vertexCount, false);

    // Map vertex ID to index
    std::vector<int> idToIndex(vertexCount);
    for (int i = 0; i < vertexCount; ++i) {
        idToIndex[i] = vertexIds[i];
    }

    while (!unvisited.empty()) {
        // Get vertex with minimum distance
        auto it = unvisited.begin();
        int current = it->second;
        unvisited.erase(it);
        result.iterations++;  // Count: extracting vertex from priority queue

        // Find index for current vertex
        int currentIndex = -1;
        for (int i = 0; i < vertexCount; ++i) {
            if (vertexIds[i] == current) {
                currentIndex = i;
                break;
            }
        }

        if (currentIndex == -1) {
            continue;
        }

        if (finalized[currentIndex]) {
            continue;
        }

        finalized[currentIndex] = true;

        // If we reached the target, we can stop (optional optimization)
        if (targetVertex.has_value() && current == targetVertex.value()) {
            result.targetDistance = result.distances[current];
            result.shortestPath = reconstructPath(startVertex, current, result.predecessors);
            return result;
        }

        // Relax edges - count each edge examination
        for (const auto& [neighbor, weight] : m_graph.neighbors(current)) {
            result.iterations++;  // Count: examining an edge

            // Find index for neighbor
            int neighborIndex = -1;
            for (int i = 0; i < vertexCount; ++i) {
                if (vertexIds[i] == neighbor) {
                    neighborIndex = i;
                    break;
                }
            }

            if (neighborIndex == -1 || finalized[neighborIndex]) {
                continue;
            }

            double newDist = result.distances[current] + weight;

            if (newDist < result.distances[neighbor]) {
                result.distances[neighbor] = newDist;
                result.predecessors[neighbor] = current;
                unvisited.insert({newDist, neighbor});
                result.iterations++;  // Count: updating distance (relaxation)
            }
        }
    }
    
    // If target was specified, reconstruct path
    if (targetVertex.has_value()) {
        int target = targetVertex.value();
        if (result.distances[target] != INF) {
            result.targetDistance = result.distances[target];
            result.shortestPath = reconstructPath(startVertex, target, result.predecessors);
        }
    }
    
    return result;
}

std::vector<int> DijkstraAlgorithm::reconstructPath(int startVertex, int targetVertex,
                                                     const std::map<int, int>& predecessors) const {
    std::vector<int> path;
    
    if (startVertex == targetVertex) {
        path.push_back(startVertex);
        return path;
    }
    
    // Backtrack from target to start
    int current = targetVertex;
    while (predecessors.find(current) != predecessors.end()) {
        path.push_back(current);
        current = predecessors.at(current);
        
        if (current == startVertex) {
            path.push_back(startVertex);
            break;
        }
    }
    
    // Reverse to get start -> target order
    std::reverse(path.begin(), path.end());
    
    // Verify path starts at startVertex
    if (path.empty() || path.front() != startVertex) {
        return {};  // No valid path found
    }
    
    return path;
}
