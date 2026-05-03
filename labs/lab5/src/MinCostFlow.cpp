#include "include/MinCostFlow.h"

#include <algorithm>
#include <limits>
#include <unordered_set>

namespace {
// same idea as in max-flow:
// recover residual path by going from sink back to source
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

// helper just for pretty output
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

MinCostFlowResult MinCostFlowSolver::compute(int source, int sink, int targetFlow) {
    MinCostFlowResult result{targetFlow, 0, 0, false, {}};

    // sending 0 units is trivially successful
    if (targetFlow <= 0) {
        result.success = true;
        return result;
    }

    // bad source/sink => no answer
    if (source == sink ||
        !m_network.hasVertex(source) ||
        !m_network.hasVertex(sink)) {
        return result;
    }

    // solve min-cost-flow from clean zero state
    m_network.resetFlows();

    std::unordered_map<int, int> distances;
    std::unordered_map<int, ResidualArc> parent;
    int iteration = 1;

    // each iteration:
    // 1) find shortest path by cost in residual network
    // 2) push as much as possible through it
    while (result.achievedFlow < targetFlow &&
           dijkstra(source, sink, distances, parent)) {
        std::vector<ResidualArc> residualPath = reconstructResidualPath(source, sink, parent);
        if (residualPath.empty()) {
            break;
        }

        // initially we can still send only "what is left" to target
        int bottleneck = targetFlow - result.achievedFlow;
        int unitCost = 0;

        // then restrict it by capacities on path
        // also sum full path cost for one unit of flow
        for (const ResidualArc& arc : residualPath) {
            bottleneck = std::min(bottleneck, arc.residualCapacity);
            unitCost += arc.cost;
        }

        // just safety
        if (bottleneck <= 0) {
            break;
        }

        // push flow through all arcs of this path
        for (const ResidualArc& arc : residualPath) {
            m_network.augment(arc.edgeIndex, arc.forward, bottleneck);
        }

        // update totals
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

    result.success = result.achievedFlow >= targetFlow;
    return result;
}

bool MinCostFlowSolver::dijkstra(int source, int sink,
                                 std::unordered_map<int, int>& distances,
                                 std::unordered_map<int, ResidualArc>& parent) const
{
    constexpr int INF = std::numeric_limits<int>::max();
    distances.clear();
    parent.clear();

    // same guard as in original dijkstra
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
    std::vector<int> T(p, INF);
    
    // X[v] -- mark: 0 = unvisited, 1 = visited (X : array [1..p] of 0..1)
    std::vector<int> X(p, 0);
    
    // H[v] -- predecessor of v on shortest path (H : array [1..p] of 0..p)
    std::vector<int> H(p, -1);
    // extra thing for flow lab:
    // remember the exact residual arc, not only predecessor vertex
    std::vector<std::optional<ResidualArc>> parentArcs(p, std::nullopt);

    int s = source;
    int t = sink;

    int startIndex = idToIndex[s];
    T[startIndex] = 0;        // T[s] := 0
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
        int m = INF;
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
        // parent for flow solver = exact residual arc to this vertex
        if (parentArcs[i].has_value()) {
            parent[vertexIds[i]] = parentArcs[i].value();
        }
    }

    auto sinkIt = idToIndex.find(sink);
    return sinkIt != idToIndex.end() && T[sinkIt->second] != INF;
}
