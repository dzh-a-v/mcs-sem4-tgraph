#include "include/BFS.h"
#include <queue>
#include <vector>
#include <algorithm>

BreadthFirstSearch::BreadthFirstSearch(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

BFSResult BreadthFirstSearch::traverseWithLevels(int startVertex) {
    BFSResult result;
    result.iterations = 0;
    result.maxLevel = 0;
    
    if (!m_graph.hasVertex(startVertex)) {
        return result;
    }
    
    std::vector<int> vertexIds = m_graph.vertexIds();
    std::vector<bool> visited(m_graph.size(), false);
    
    // Map vertex ID to index
    std::vector<int> idToIndexMap(m_graph.size());
    for (size_t i = 0; i < vertexIds.size(); ++i) {
        idToIndexMap[i] = vertexIds[i];
    }
    
    // Queue stores {vertex, level}
    std::queue<std::pair<int, int>> queue;
    queue.push({startVertex, 0});
    
    // Find index for startVertex
    int startIndex = -1;
    for (size_t i = 0; i < vertexIds.size(); ++i) {
        if (vertexIds[i] == startVertex) {
            startIndex = static_cast<int>(i);
            break;
        }
    }
    
    if (startIndex == -1) {
        return result;
    }
    
    visited[startIndex] = true;
    result.iterations++;  // Count: processing start vertex
    result.traversalOrder.push_back(startVertex);
    result.levels[0].push_back(startVertex);

    while (!queue.empty()) {
        auto [current, level] = queue.front();
        queue.pop();

        result.iterations++;  // Count: extracting vertex from queue
        result.maxLevel = std::max(result.maxLevel, level);

        // Visit all neighbors - count each edge examination
        for (const auto& [neighbor, weight] : m_graph.neighbors(current)) {
            result.iterations++;  // Count: examining an edge

            // Find index for neighbor
            int neighborIndex = -1;
            for (size_t i = 0; i < vertexIds.size(); ++i) {
                if (vertexIds[i] == neighbor) {
                    neighborIndex = static_cast<int>(i);
                    break;
                }
            }

            if (neighborIndex != -1 && !visited[neighborIndex]) {
                visited[neighborIndex] = true;
                result.traversalOrder.push_back(neighbor);
                result.levels[level + 1].push_back(neighbor);
                queue.push({neighbor, level + 1});
            }
        }
    }
    
    return result;
}
