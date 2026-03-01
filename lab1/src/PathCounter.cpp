#include "include/PathCounter.h"
#include <unordered_map>

namespace graph {

PathCounter::PathCounter(Graph const& graph) : m_graph_(graph) {}

/// Returns the total count of simple paths between two vertices.
int PathCounter::getPathCount(int from, int to) {
    return static_cast<int>(getAllPaths(from, to).size());
}

/// Finds all simple paths between two vertices using backtracking.
/// Returns empty matrix if either vertex doesn't exist in the graph.
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

/// Recursive backtracking algorithm to enumerate all simple paths.
/// Marks vertices as visited to avoid cycles, then unmarks them on backtrack
/// to allow exploring alternative paths through the same vertex.
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