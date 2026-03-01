#include "include/PathCounter.h"
#include <unordered_map>

namespace graph {

PathCounter::PathCounter(Graph const& graph) : m_graph_(graph) {}

int PathCounter::getPathCount(int from, int to) {
    return static_cast<int>(getAllPaths(from, to).size());
}

PathMatrix PathCounter::getAllPaths(int from, int to) {
    if (!m_graph_.hasVertex(from) || !m_graph_.hasVertex(to)) return {};
    PathMatrix all_paths;
    std::vector<int> current_path;
    std::vector<bool> visited(m_graph_.size(), false);

    auto ids = m_graph_.vertexIds();
    std::unordered_map<int, int> idToIdx;
    for(int i=0; i<ids.size(); ++i) idToIdx[ids[i]] = i;

    backtrackAllPaths(from, to, visited, current_path, all_paths);
    return all_paths;
}

void PathCounter::backtrackAllPaths(int current, int target, std::vector<bool>& visited,
                          std::vector<int>& currentPath, PathMatrix& allPaths) {
    currentPath.push_back(current);
    visited[current] = true;

    if (current == target) {
        allPaths.push_back(currentPath);
    } else {
        for (auto const& [neighborId, _] : m_graph_.neighbors(current)) {
            if (!visited[neighborId]) {
                backtrackAllPaths(neighborId, target, visited, currentPath, allPaths);
            }
        }
    }

    currentPath.pop_back();
    visited[current] = false;
}
}