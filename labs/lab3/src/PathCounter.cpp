#include "include/PathCounter.h"
#include <unordered_map>

SimplePathFinder::SimplePathFinder(AdjacencyGraph const& graph) 
    : m_graph(graph) 
{}

int SimplePathFinder::countPaths(int from, int to) { // kostyl' :P
    PathCollection paths = findAllPaths(from, to);
    return static_cast<int>(paths.size());
}

PathCollection SimplePathFinder::findAllPaths(int from, int to) {
    // Validate vertices exist
    if (!m_graph.hasVertex(from) || !m_graph.hasVertex(to)) {
        return {};
    }
    
    PathCollection collectedPaths;
    std::vector<int> currentPath;
    std::vector<bool> visited(m_graph.size(), false);
    
    // MBuild vcertex ID → index mapping 
    auto vertexList = m_graph.vertexIds();
    std::unordered_map<int, int> idToIndex;
    for (int i = 0; i < static_cast<int>(vertexList.size()); ++i) {
        idToIndex[vertexList[i]] = i; // it's for the future
    } // when IDs may not be like "|N"
    
    dfsExplore(from, to, visited, currentPath, collectedPaths);
    return collectedPaths;
}

void SimplePathFinder::dfsExplore(
    int current, 
    int target, 
    std::vector<bool>& visited,
    std::vector<int>& path, 
    PathCollection& allPaths) 
{
    // Add current vertex to path
    path.push_back(current);
    visited[current] = true;
    
    // Check if reached destination
    if (current == target) {
        allPaths.push_back(path);
    } else {
        // Explore unvisited neighbors
        for (const auto& [neighborId, edgeWeight] : m_graph.neighbors(current)) {
            if (!visited[neighborId]) {
                dfsExplore(neighborId, target, visited, path, allPaths);
            }
        }
    }
    
    // Backtrack
    path.pop_back();
    visited[current] = false;
}
