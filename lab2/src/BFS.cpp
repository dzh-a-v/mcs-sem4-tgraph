#include "include/BFS.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <unordered_map>

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

    // Build hash map for O(1) vertex ID to index lookup
    std::unordered_map<int, int> idToIndex;
    for (int i = 0; i < m_graph.size(); ++i) {
        result.iterations++;
        idToIndex[vertexIds[i]] = i;
    }

    // Queue stores {vertex, level}
    std::queue<std::pair<int, int>> queue;
    queue.push({startVertex, 0});
    result.iterations++;  // Count: queue push (O(1) operation)

    int startIndex = idToIndex[startVertex];

    if (startIndex == -1) {
        return result;
    }

    visited[startIndex] = true;
    result.traversalOrder.push_back(startVertex);
    result.levels[0].push_back(startVertex);

    while (!queue.empty()) {
        auto [current, level] = queue.front();
        queue.pop();
        result.iterations++;  // Count: queue pop (O(1) operation)

        result.maxLevel = std::max(result.maxLevel, level);

        // Visit all neighbors
        for (const auto& [neighbor, weight] : m_graph.neighbors(current)) {
            result.iterations++;

            // O(1) lookup using hash map
            auto it = idToIndex.find(neighbor);
            if (it != idToIndex.end()) {
                int neighborIndex = it->second;
                if (!visited[neighborIndex]) {
                    visited[neighborIndex] = true;
                    result.traversalOrder.push_back(neighbor);
                    result.levels[level + 1].push_back(neighbor);
                    queue.push({neighbor, level + 1});
                    result.iterations++;  // Count: queue push (O(1) operation)
                }
            }
        }
    }

    return result;
}
