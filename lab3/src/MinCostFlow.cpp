#include "include/MinCostFlow.h"

#include <algorithm>
#include <limits>
#include <unordered_set>

namespace {
constexpr double kEps = 1e-9;

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

MinCostFlowSolver::MinCostFlowSolver(FlowNetwork& network)
    : m_network(network) {}

MinCostFlowResult MinCostFlowSolver::compute(int source, int sink, double targetFlow) {
    MinCostFlowResult result{targetFlow, 0.0, 0.0, false, {}};

    if (targetFlow <= 0.0) {
        result.success = true;
        return result;
    }

    if (source == sink ||
        !m_network.hasVertex(source) ||
        !m_network.hasVertex(sink)) {
        return result;
    }

    m_network.resetFlows();

    std::unordered_map<int, double> distances;
    std::unordered_map<int, ResidualArc> parent;
    int iteration = 1;

    while (result.achievedFlow + kEps < targetFlow &&
           dijkstra(source, sink, distances, parent)) {
        std::vector<ResidualArc> residualPath = reconstructResidualPath(source, sink, parent);
        if (residualPath.empty()) {
            break;
        }

        double bottleneck = targetFlow - result.achievedFlow;
        double unitCost = 0.0;

        for (const ResidualArc& arc : residualPath) {
            bottleneck = std::min(bottleneck, arc.residualCapacity);
            unitCost += arc.cost;
        }

        if (bottleneck <= kEps) {
            break;
        }

        for (const ResidualArc& arc : residualPath) {
            m_network.augment(arc.edgeIndex, arc.forward, bottleneck);
        }

        result.achievedFlow += bottleneck;
        result.totalCost += bottleneck * unitCost;
        result.steps.push_back({
            iteration++,
            toVertexPath(source, residualPath),
            bottleneck,
            unitCost,
            result.achievedFlow,
            result.totalCost
        });
    }

    result.success = result.achievedFlow + kEps >= targetFlow;
    return result;
}

bool MinCostFlowSolver::dijkstra(int source, int sink,
                                 std::unordered_map<int, double>& distances,
                                 std::unordered_map<int, ResidualArc>& parent) const
{
    const double infinity = std::numeric_limits<double>::infinity();
    std::unordered_set<int> visited;

    distances.clear();
    parent.clear();

    for (int vertexId : m_network.vertexIds()) {
        distances[vertexId] = infinity;
    }
    distances[source] = 0.0;

    while (visited.size() < static_cast<size_t>(m_network.size())) {
        int current = -1;
        double bestDistance = infinity;

        // Follow the same Dijkstra-style selection logic as elsewhere in the lab,
        // even when some costs are negative by the lab convention.
        for (int vertexId : m_network.vertexIds()) {
            if (visited.find(vertexId) == visited.end() &&
                distances[vertexId] < bestDistance) {
                bestDistance = distances[vertexId];
                current = vertexId;
            }
        }

        if (current == -1) {
            break;
        }

        visited.insert(current);
        if (current == sink) {
            break;
        }

        for (const ResidualArc& arc : m_network.residualNeighbors(current)) {
            if (visited.find(arc.to) != visited.end()) {
                continue;
            }

            double candidateDistance = distances[current] + arc.cost;
            if (candidateDistance < distances[arc.to]) {
                distances[arc.to] = candidateDistance;
                parent[arc.to] = arc;
            }
        }
    }

    return distances[sink] != infinity;
}
