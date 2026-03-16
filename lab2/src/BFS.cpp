#include "include/BFS.h"
#include <queue>
#include <vector>
#include <algorithm>

BreadthFirstSearch::BreadthFirstSearch(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

BFSResult BreadthFirstSearch::traverse(int startVertex) {
    BFSResult result;
    result.iterations = 0;
    
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
    
    std::queue<int> queue;
    queue.push(startVertex);
    
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
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        
        result.traversalOrder.push_back(current);
        result.iterations++;
        
        // Visit all neighbors
        for (const auto& [neighbor, weight] : m_graph.neighbors(current)) {
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
                queue.push(neighbor);
            }
        }
    }
    
    return result;
}

BFSResult BreadthFirstSearch::traverseLevels(int startVertex) {
    // Same as traverse, but can be extended to show levels
    return traverse(startVertex);
}
