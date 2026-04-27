#include "include/MaxFlowSolver.h"

#include <algorithm>
#include <limits>
#include <queue>
#include <unordered_set>

namespace {
std::vector<ResidualArc> reconstructResidualPath(
    int source,
    int sink,
    const std::unordered_map<int, ResidualArc>& parent)
{
    std::vector<ResidualArc> path;
    int current = sink;

    while (current != source) {
        auto it = parent.find(current);
        if (it == parent.end()) {
            return {};
        }
        path.push_back(it->second);
        current = it->second.from;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<int> toVertexPath(int source, const std::vector<ResidualArc>& residualPath) {
    std::vector<int> path;
    path.push_back(source);

    for (const ResidualArc& arc : residualPath) {
        path.push_back(arc.to);
    }

    return path;
}
}

MaxFlowSolver::MaxFlowSolver(FlowNetwork& network)
    : m_network(network) {}

MaxFlowResult MaxFlowSolver::compute(int source, int sink) {
    MaxFlowResult result{0, {}};

    if (source == sink ||
        !m_network.hasVertex(source) ||
        !m_network.hasVertex(sink)) {
        return result;
    }

    m_network.resetFlows();

    std::unordered_map<int, ResidualArc> parent;
    int iteration = 1;

    while (bfs(source, sink, parent)) {
        std::vector<ResidualArc> residualPath = reconstructResidualPath(source, sink, parent);
        if (residualPath.empty()) {
            break;
        }

        int bottleneck = std::numeric_limits<int>::max();
        for (const ResidualArc& arc : residualPath) {
            bottleneck = std::min(bottleneck, arc.residualCapacity);
        }

        for (const ResidualArc& arc : residualPath) {
            m_network.augment(arc.edgeIndex, arc.forward, bottleneck);
        }

        result.maxFlow += bottleneck;
        result.steps.push_back({
            iteration++,
            toVertexPath(source, residualPath),
            bottleneck,
            result.maxFlow
        });
    }

    return result;
}

bool MaxFlowSolver::bfs(int source, int sink,
                        std::unordered_map<int, ResidualArc>& parent) const
{
    std::queue<int> queue;
    std::unordered_set<int> visited;

    parent.clear();
    queue.push(source);
    visited.insert(source);

    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();

        for (const ResidualArc& arc : m_network.residualNeighbors(current)) {
            if (visited.find(arc.to) != visited.end()) {
                continue;
            }

            visited.insert(arc.to);
            parent[arc.to] = arc;

            if (arc.to == sink) {
                return true;
            }

            queue.push(arc.to);
        }
    }

    return false;
}
