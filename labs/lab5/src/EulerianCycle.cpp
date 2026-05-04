#include "include/EulerianCycle.h"
#include <algorithm>
#include <functional>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>

EulerianCycleBuilder::EulerianCycleBuilder(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

namespace {

// ----- Helpers ---------------------------------------------------------------

// Make a writable copy of the input graph.  Eulerization may add edges, and
// we do not want to mutate the user's graph in place.
std::unique_ptr<AdjacencyGraph> cloneGraph(const AdjacencyGraph& g) {
    auto copy = std::make_unique<AdjacencyGraph>(g.isDirected());
    for (int v : g.vertexIds()) {
        copy->addVertex(v);
    }
    for (const WeightedEdge& e : g.edges()) {
        copy->addEdge(e.from, e.to, e.weight);
    }
    return copy;
}

// Degree of v in the (multi-)graph: just the size of its adjacency list, since
// addEdge() pushes one entry per endpoint per occurrence.
int degreeOf(const AdjacencyGraph& g, int v) {
    return static_cast<int>(g.neighbors(v).size());
}

// Find connected components, but only count vertices that touch at least one
// edge.  An Eulerian cycle does not need to "visit" isolated vertices, so
// they should not force us to add bridges to them.
//
// Returns: list of components, each component being a list of vertex IDs.
std::vector<std::vector<int>> connectedComponentsWithEdges(const AdjacencyGraph& g) {
    std::vector<std::vector<int>> components;
    std::unordered_set<int> visited;

    for (int start : g.vertexIds()) {
        if (visited.count(start)) continue;
        if (g.neighbors(start).empty()) continue;  // skip isolated vertices

        // Standard BFS over the undirected graph.
        std::vector<int> component;
        std::vector<int> queue{start};
        visited.insert(start);
        size_t head = 0;
        while (head < queue.size()) {
            int u = queue[head++];
            component.push_back(u);
            for (const auto& [w, _wt] : g.neighbors(u)) {
                if (!visited.count(w)) {
                    visited.insert(w);
                    queue.push_back(w);
                }
            }
        }
        components.push_back(std::move(component));
    }
    return components;
}

// Hierholzer's algorithm for an undirected (multi-)graph.
//
// Idea: starting at `start`, walk along edges (each used at most once); when
// stuck, you are guaranteed to be back at `start` because every vertex has
// even degree.  If unused edges remain, you must have visited some vertex on
// the current path that still has free edges -- splice a sub-cycle in there.
//
// Implementation: iterative, using a stack of vertices.  We maintain, per
// vertex, an "edge iterator" pointing to the next adjacency-list slot that
// has not yet been consumed, plus a side `used` set keyed by edge IDs so the
// peer endpoint can also see that the edge is gone.
std::vector<int> hierholzer(const AdjacencyGraph& g, int start) {
    // Pull adjacency lists into a mutable structure where each entry has a
    // unique "edge id" so the same edge can be marked used from both sides.
    struct AdjSlot {
        int neighbor;
        int edgeId;
    };

    std::unordered_map<int, std::vector<AdjSlot>> adj;
    int nextId = 0;

    // We need the same edgeId for the (u,v) and (v,u) entries that came from
    // the SAME addEdge call.  AdjacencyGraph::edges() lists each undirected
    // edge once with from < to; iterate that list and assign one id per edge.
    for (const WeightedEdge& e : g.edges()) {
        const int id = nextId++;
        adj[e.from].push_back({e.to, id});
        adj[e.to].push_back({e.from, id});
    }

    std::unordered_set<int> usedEdges;       // edges already consumed
    std::unordered_map<int, size_t> cursor;  // next index to inspect in adj[v]

    std::stack<int> path;       // current incomplete walk
    std::vector<int> cycle;     // result, built in reverse order
    
    ////////// S := ∅ { стек для хранения вершин }
    ////////// select v ∈ V { произвольная вершина }
    path.push(start);
    ////////// v → S { положить v в стек S }

    // while S ≠ ∅ do
    while (!path.empty()) {
        ////////// while S ≠ ∅ do
        
        int u = path.top();
        ////////// v := top S { v — верхний элемент стека }
        
        auto& list = adj[u];
        size_t& i = cursor[u];

        // Skip past edges that have already been used (possibly by the other
        // endpoint).  This loop ends either at an unused edge or at end().
        while (i < list.size() && usedEdges.count(list[i].edgeId)) {
            ++i;
        }

        if (i == list.size()) {
            // Stuck at u: no outgoing unused edges left.  Move u into the
            // result and continue from whatever is below it on the stack.
            // Building the cycle this way (popping) naturally handles the
            // splice case -- subcycles get inserted in the right place.
            cycle.push_back(u);
            path.pop();
            ////////// if Γ[v] = ∅ then
            //////////     v ← S; yield v { очередная вершина эйлерова цикла }
        } else {
            // Take this edge: walk to the neighbor, mark the edge consumed.
            const AdjSlot slot = list[i];
            usedEdges.insert(slot.edgeId);
            ++i;
            path.push(slot.neighbor);
            ////////// else
            //////////     select u ∈ Γ[v] { взять первую вершину из списка смежности }
            //////////     u → S { положить u в стек }
            //////////     Γ[v] := Γ[v] - u; Γ[u] := Γ[u] - v { удалить ребро (v,u) }
            ////////// end if
        }
    }
    ////////// end while

    // We were popping, so the cycle is currently start <- ... <- start.
    // Reverse to get the natural start -> ... -> start order.
    std::reverse(cycle.begin(), cycle.end());
    return cycle;
    ////////// Выход: последовательность вершин эйлерова цикла
}

// Set of unordered pairs (u,v) representing the edges already present in the
// graph.  Used to detect when adding (u,v) would duplicate an existing edge.
std::set<std::pair<int,int>> existingEdgePairs(const AdjacencyGraph& g) {
    std::set<std::pair<int,int>> pairs;
    for (const WeightedEdge& e : g.edges()) {
        int a = e.from, b = e.to;
        if (a > b) std::swap(a, b);
        pairs.insert({a, b});
    }
    return pairs;
}

// Try to find a perfect matching of `oddVertices` such that no matched pair
// (u,v) already exists in `forbidden`.  Returns true on success and fills
// `pairs` with the matching.  Uses backtracking; the number of odd vertices
// in our use cases is small enough for this to be fine.
bool findNonMultigraphMatching(const std::vector<int>& oddVertices,
                               const std::set<std::pair<int,int>>& forbidden,
                               std::vector<std::pair<int,int>>& pairs) {
    const size_t n = oddVertices.size();
    std::vector<bool> used(n, false);

    auto isForbidden = [&](int u, int v) {
        int a = u, b = v;
        if (a > b) std::swap(a, b);
        return forbidden.count({a, b}) > 0;
    };

    // Recursive helper: pick the lowest-index unused vertex, try every
    // unused partner that does not collide with an existing edge.
    std::function<bool()> recurse = [&]() -> bool {
        // Find first unused index.
        size_t i = 0;
        while (i < n && used[i]) ++i;
        if (i == n) return true;  // all matched

        used[i] = true;
        for (size_t j = i + 1; j < n; ++j) {
            if (used[j]) continue;
            if (isForbidden(oddVertices[i], oddVertices[j])) continue;

            used[j] = true;
            pairs.push_back({oddVertices[i], oddVertices[j]});
            if (recurse()) return true;
            pairs.pop_back();
            used[j] = false;
        }
        used[i] = false;
        return false;
    };

    pairs.clear();
    return recurse();
}

}  // namespace

EulerianCycleResult EulerianCycleBuilder::compute(EulerizationMode mode) const {
    EulerianCycleResult result;
    result.success = false;
    result.wasAlreadyEulerian = false;
    result.requiresMultigraph = false;

    // Always work on a copy so we can add edges freely.
    result.modifiedGraph = cloneGraph(m_graph);
    AdjacencyGraph& G = *result.modifiedGraph;

    if (G.isDirected()) {
        // Per the lab spec we only handle undirected graphs.
        return result;
    }

    if (G.edges().empty()) {
        // No edges -> trivially "Eulerian" with an empty cycle, but there is
        // nothing meaningful to walk.  Surface this as not-success so the
        // caller can print a sensible message.
        return result;
    }

    // ---- Step 1. Make sure the edge-bearing part of the graph is connected.
    //
    // If we have several components that each contain edges, we must add
    // bridge edges between them.  Connecting k components needs k-1 bridges.
    // We always bridge the first vertex of each component (deterministic).
    //
    // Bridging two different components can never create a parallel edge
    // (the endpoints had no path between them, let alone a direct edge), so
    // step 1 is safe in either mode.
    while (true) {
        auto components = connectedComponentsWithEdges(G);
        if (components.size() <= 1) break;

        const int u = components[0].front();
        const int v = components[1].front();
        G.addEdge(u, v, 0.0);  // weight 0: we made this edge up
        result.additions.push_back({u, v, "connect components"});
    }

    // ---- Step 2. Fix odd-degree vertices by pairing.
    //
    // Handshaking lemma: there is always an even number of odd-degree
    // vertices, so pairing always succeeds.  Each added edge flips the
    // parity of its two endpoints, turning two odd vertices into even ones.
    std::vector<int> oddVertices;
    for (int v : G.vertexIds()) {
        if (degreeOf(G, v) % 2 == 1) {
            oddVertices.push_back(v);
        }
    }

    // Sort for reproducibility -- same input should always pair the same way.
    std::sort(oddVertices.begin(), oddVertices.end());

    // Try to pair odd vertices.  In NonMultigraphOnly mode we look for a
    // perfect matching that avoids any pair already connected by an edge.
    // If no such matching exists, we cannot fix the graph without making it
    // a multigraph -- bail out and let the caller decide whether to retry.
    if (!oddVertices.empty()) {
        std::vector<std::pair<int,int>> pairs;
        bool foundClean = false;

        if (mode == EulerizationMode::NonMultigraphOnly) {
            const auto forbidden = existingEdgePairs(G);
            foundClean = findNonMultigraphMatching(oddVertices, forbidden, pairs);

            if (!foundClean) {
                // Cannot eulerize without creating parallel edges.  Surface
                // this to the caller; do NOT mutate the graph any further
                // and do NOT compute a cycle.
                result.requiresMultigraph = true;
                result.success = false;
                // Roll back the modifiedGraph to the post-step-1 state we
                // already have, but leave additions intact so the caller can
                // see the bridge edges that would still be needed.
                return result;
            }
        } else {
            // AllowMultigraph: just pair adjacent odd vertices in the sorted
            // list.  This matches the original behavior and is guaranteed to
            // succeed (handshaking lemma).
            for (size_t i = 0; i + 1 < oddVertices.size(); i += 2) {
                pairs.push_back({oddVertices[i], oddVertices[i + 1]});
            }
        }

        // Apply the chosen pairing.
        for (const auto& [u, v] : pairs) {
            G.addEdge(u, v, 0.0);
            result.additions.push_back({u, v, "fix odd parity"});
        }
    }

    result.wasAlreadyEulerian = result.additions.empty();

    // ---- Step 3. Pick a start vertex (any vertex incident to some edge)
    // and run Hierholzer's algorithm.
    int start = -1;
    for (int v : G.vertexIds()) {
        if (!G.neighbors(v).empty()) {
            start = v;
            break;
        }
    }
    if (start == -1) {
        // Should not happen: we returned earlier on edge-empty graphs.
        return result;
    }

    result.cycle = hierholzer(G, start);
    result.success = true;
    return result;
}