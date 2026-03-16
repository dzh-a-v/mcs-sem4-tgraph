#include "include/Dijkstra.h"
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <vector>

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
    int p = m_graph.size();  // number of vertices (p in the algorithm)

    // Build hash map for vertex ID to index (1-based to match algorithm)
    std::unordered_map<int, int> idToIndex;
    for (int i = 0; i < p; ++i) {
        result.iterations++;
        idToIndex[vertexIds[i]] = i;
    }

    // T[v] -- distance from s to v (T : array [1..p] of real)
    std::vector<double> T(p, INF);
    
    // X[v] -- mark: 0 = unvisited, 1 = visited (X : array [1..p] of 0..1)
    std::vector<int> X(p, 0);
    
    // H[v] -- predecessor of v on shortest path (H : array [1..p] of 0..p)
    std::vector<int> H(p, -1);

    int s = startVertex;
    int t = targetVertex.value_or(-1);

    int startIndex = idToIndex[s];
    T[startIndex] = 0;      // T[s] := 0
    X[startIndex] = 1;      // X[s] := 1 (s is known)

    int v = s;              // v := s (current vertex)
    int vIndex = startIndex;

    // Label M: update labels
    while (true) {
        result.iterations++;  // Count: main loop iteration

        // for u ∈ Γ(v) do -- examine all neighbors of v
        for (const auto& [neighbor, weight] : m_graph.neighbors(v)) {
            result.iterations++;  // Count: examining edge

            auto neighborIt = idToIndex.find(neighbor);
            if (neighborIt == idToIndex.end()) {
                continue;
            }
            int uIndex = neighborIt->second;

            // if X[u] = 0 & T[u] > T[v] + C[v, u] then
            if (X[uIndex] == 0 && T[uIndex] > T[vIndex] + weight) {
                T[uIndex] = T[vIndex] + weight;  // T[u] := T[v] + C[v, u]
                H[uIndex] = vIndex;               // H[u] := v
                result.distances[neighbor] = T[uIndex];
                result.predecessors[neighbor] = v;
            }
        }

        // m := ∞; v := 0
        double m = INF;
        v = 0;
        vIndex = -1;

        // for u from 1 to p do -- find vertex with minimum T
        for (int u = 0; u < p; ++u) {
            result.iterations++;  // Count: finding minimum

            // if X[u] = 0 & T[u] < m then
            if (X[u] == 0 && T[u] < m) {
                vIndex = u;
                m = T[u];
                v = vertexIds[u];
            }
        }

        // if v = 0 then stop -- no path from s to t
        if (vIndex == -1) {
            break;
        }

        // if v = t then stop -- shortest path from s to t found
        if (targetVertex.has_value() && v == t) {
            result.targetDistance = T[vIndex];
            result.shortestPath = reconstructPath(s, v, H, vertexIds);
            break;
        }

        X[vIndex] = 1;  // X[v] := 1 -- shortest path from s to v found
        vIndex = vIndex;
    }

    // If target was specified, reconstruct path
    if (targetVertex.has_value()) {
        int target = targetVertex.value();
        auto targetIt = idToIndex.find(target);
        if (targetIt != idToIndex.end() && T[targetIt->second] != INF) {
            result.targetDistance = T[targetIt->second];
            result.shortestPath = reconstructPath(s, target, H, vertexIds);
        }
    }

    // Copy final distances to result
    for (int i = 0; i < p; ++i) {
        if (T[i] != INF) {
            result.distances[vertexIds[i]] = T[i];
        }
    }

    return result;
}

std::vector<int> DijkstraAlgorithm::reconstructPath(int startVertex, int targetVertex,
                                                     const std::vector<int>& H,
                                                     const std::vector<int>& vertexIds) const {
    std::vector<int> path;

    if (startVertex == targetVertex) {
        path.push_back(startVertex);
        return path;
    }

    // Find target index
    auto targetIt = std::find(vertexIds.begin(), vertexIds.end(), targetVertex);
    if (targetIt == vertexIds.end()) {
        return {};
    }
    int currentIdx = std::distance(vertexIds.begin(), targetIt);

    // Backtrack using H
    while (currentIdx != -1) {
        path.push_back(vertexIds[currentIdx]);
        if (vertexIds[currentIdx] == startVertex) {
            break;
        }
        currentIdx = H[currentIdx];
    }

    // Reverse to get start -> target order
    std::reverse(path.begin(), path.end());

    // Verify path starts at startVertex
    if (path.empty() || path.front() != startVertex) {
        return {};
    }

    return path;
}
