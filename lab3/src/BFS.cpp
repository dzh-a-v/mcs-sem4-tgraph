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

    // Build hash map for O(1) vertex ID to index lookup. FTF (now no need,
    // rn all the verteces are indexed from 0 to the last vertex.
    std::unordered_map<int, int> idToIndex;
    for (int i = 0; i < m_graph.size(); ++i) {
        result.iterations++;
        idToIndex[vertexIds[i]] = i;
    }

    // Queue stores {vertex, level}
    std::queue<std::pair<int, int>> queue;
    queue.push({startVertex, 0});
    //result.iterations++;

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

        /// Visit all neighbors
        for (const auto& [neighbor, weight] : m_graph.neighbors(current)) { 
            // loop through all vertices connected to current + unpacking edge 
            // (connected vertex + weight)
            result.iterations++;

            auto it = idToIndex.find(neighbor); // find the index of this neighbor vertex
            // (also FTF)
            //if (it != idToIndex.end()) { 
                int neighborIndex = it->second; // 
                if (!visited[neighborIndex]) { 
                    visited[neighborIndex] = true; 
                    result.traversalOrder.push_back(neighbor); 
                    result.levels[level + 1].push_back(neighbor); // next depth
                    queue.push({neighbor, level + 1}); // we will process its neighbors lager
                    result.iterations++;
                }
            //}
        }
    }

    return result;
}
