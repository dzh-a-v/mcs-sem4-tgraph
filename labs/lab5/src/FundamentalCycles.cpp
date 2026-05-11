#include "include/FundamentalCycles.h"
#include <algorithm>
#include <set>
#include <unordered_map>

FundamentalCycleSystem::FundamentalCycleSystem(const AdjacencyGraph& originalGraph,
                                               const AdjacencyGraph& spanningTree)
    : m_graph(originalGraph)
    , m_tree(spanningTree)
{}

namespace {

// Normalize an edge so the smaller endpoint comes first.  This lets us treat
// (u,v) and (v,u) as the same edge when comparing cycles.
std::pair<int,int> normEdge(int u, int v) {
    if (u > v) std::swap(u, v);
    return {u, v};
}

// In a tree there is exactly one simple path between any two vertices.
// BFS from `start` until we hit `target`, recording where we came from so
// we can reconstruct the path by walking predecessors backwards.
//
// Returns the path as a vertex sequence: [start, ..., target].  Empty if
// target is unreachable (which should not happen for a spanning tree).
std::vector<int> findTreePath(const AdjacencyGraph& tree, int start, int target) {
    std::unordered_map<int, int> parent;  // child -> parent
    parent[start] = start;                // sentinel: start is its own parent

    std::vector<int> queue{start};
    size_t head = 0;
    bool found = (start == target);

    while (head < queue.size() && !found) {
        int u = queue[head++];
        for (const auto& [v, _w] : tree.neighbors(u)) {
            if (parent.count(v)) continue;  // already visited
            parent[v] = u;
            if (v == target) {
                found = true;
                break;
            }
            queue.push_back(v);
        }
    }

    if (!found) return {};

    // Walk back from target to start, then reverse.
    std::vector<int> path;
    int cur = target;
    while (cur != start) {
        path.push_back(cur);
        cur = parent[cur];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

// Collect edges of the spanning tree as a set of normalized (u,v) pairs.
// Used to decide which graph edges are tree edges vs chords.
std::set<std::pair<int,int>> collectTreeEdges(const AdjacencyGraph& tree) {
    std::set<std::pair<int,int>> result;
    for (const WeightedEdge& e : tree.edges()) {
        result.insert(normEdge(e.from, e.to));
    }
    return result;
}

}  // namespace

std::vector<FundamentalCycle> FundamentalCycleSystem::compute() const {
    std::vector<FundamentalCycle> cycles;

    const auto treeEdgeSet = collectTreeEdges(m_tree);

    // Walk every graph edge.  If it is NOT in the tree, it is a chord, and
    // it defines exactly one fundamental cycle.
    for (const WeightedEdge& e : m_graph.edges()) {
        const auto key = normEdge(e.from, e.to);
        if (treeEdgeSet.count(key) > 0) {
            continue;  // tree edge -- skip
        }

        // Chord: build its fundamental cycle.
        FundamentalCycle cycle;
        cycle.chordFrom = e.from;
        cycle.chordTo = e.to;

        // Tree path from one endpoint of the chord to the other.
        // Combined with the chord itself, this closes the cycle.
        std::vector<int> path = findTreePath(m_tree, e.from, e.to);
        if (path.empty()) continue;  // disconnected tree -- no cycle possible

        // Vertex sequence for display: tree path + close back to start.
        cycle.vertexSequence = path;
        cycle.vertexSequence.push_back(path.front());  // close the loop

        // Edge set: tree-path edges + the chord itself.
        for (size_t i = 0; i + 1 < path.size(); ++i) {
            cycle.edges.push_back(normEdge(path[i], path[i + 1]));
        }
        cycle.edges.push_back(key);  // the chord

        // Sort for deterministic display and to make XOR easier (linear merge).
        std::sort(cycle.edges.begin(), cycle.edges.end());
        cycles.push_back(std::move(cycle));
    }

    return cycles;
}

std::vector<std::pair<int,int>> FundamentalCycleSystem::symmetricDifference(
    const std::vector<std::pair<int,int>>& a,
    const std::vector<std::pair<int,int>>& b)
{
    // Both inputs are sorted (compute() sorts each cycle), so the symmetric
    // difference is a single linear merge: keep elements that are in exactly
    // one of the two lists.
    std::vector<std::pair<int,int>> result;
    size_t i = 0;
    size_t j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] == b[j]) {
            // present in both -> drop
            ++i;
            ++j;
        } else if (a[i] < b[j]) {
            result.push_back(a[i++]);
        } else {
            result.push_back(b[j++]);
        }
    }
    while (i < a.size()) result.push_back(a[i++]);
    while (j < b.size()) result.push_back(b[j++]);
    return result;
}