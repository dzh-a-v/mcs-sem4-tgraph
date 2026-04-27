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
    constexpr double INF = std::numeric_limits<double>::infinity();
    distances.clear();
    parent.clear();

    if (!m_network.hasVertex(source)) {
        return false;
    }

    auto vertexIds = m_network.vertexIds();
    int p = m_network.size();  // number of vertices (p in the algorithm)

    // Build hash map for vertex ID to index (1-based to match algorithm)
    std::unordered_map<int, int> idToIndex;
    for (int i = 0; i < p; ++i) {
        idToIndex[vertexIds[i]] = i;
    }

    // T[v] -- distance from s to v (T : array [1..p] of real)
    std::vector<double> T(p, INF);
    
    // X[v] -- mark: 0 = unvisited, 1 = visited (X : array [1..p] of 0..1)
    std::vector<int> X(p, 0);
    
    // H[v] -- predecessor of v on shortest path (H : array [1..p] of 0..p)
    std::vector<int> H(p, -1);
    std::vector<std::optional<ResidualArc>> parentArcs(p, std::nullopt);

    int s = source;
    int t = sink;

    int startIndex = idToIndex[s];
    T[startIndex] = 0.0;      // T[s] := 0
    X[startIndex] = 1;        // X[s] := 1 (s is known)

    int v = s;                // v := s (current vertex)
    int vIndex = startIndex;

    // Label M: update labels
    while (true) {
        //result.iterations++;  // Count: main loop iteration

        // for u ∈ Γ(v) do -- examine all neighbors of v
        for (const ResidualArc& arc : m_network.residualNeighbors(v)) {
            auto neighborIt = idToIndex.find(arc.to);
            if (neighborIt == idToIndex.end()) {
                continue;
            }
            int uIndex = neighborIt->second;

            // if X[u] = 0 & T[u] > T[v] + C[v, u] then
            if (X[uIndex] == 0 && T[uIndex] > T[vIndex] + arc.cost) {
                T[uIndex] = T[vIndex] + arc.cost;  // T[u] := T[v] + C[v, u]
                H[uIndex] = vIndex;                // H[u] := v
                parentArcs[uIndex] = arc;
            }
        }

        // m := ∞; v := 0
        double m = INF;
        v = 0;
        vIndex = -1;

        // for u from 1 to p do -- find vertex with minimum T
        for (int u = 0; u < p; ++u) {
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
        if (v == t) {
            break;
        }

        X[vIndex] = 1;  // X[v] := 1 -- shortest path from s to v found
    }

    for (int i = 0; i < p; ++i) {
        if (T[i] != INF) {
            distances[vertexIds[i]] = T[i];
        }
        if (parentArcs[i].has_value()) {
            parent[vertexIds[i]] = parentArcs[i].value();
        }
    }

    auto sinkIt = idToIndex.find(sink);
    return sinkIt != idToIndex.end() && T[sinkIt->second] != INF;
}
