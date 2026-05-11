#include "include/EulerianCycle.h"
#include <algorithm>
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
    path.push(start);

    while (!path.empty()) {
        int u = path.top();
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
        } else {
            // Take this edge: walk to the neighbor, mark the edge consumed.
            const AdjSlot slot = list[i];
            usedEdges.insert(slot.edgeId);
            ++i;
            path.push(slot.neighbor);
        }
    }

    // We were popping, so the cycle is currently start <- ... <- start.
    // Reverse to get the natural start -> ... -> start order.
    std::reverse(cycle.begin(), cycle.end());
    return cycle;
}

}  // namespace

EulerianCycleResult EulerianCycleBuilder::compute() const {
    EulerianCycleResult result;
    result.success = false;
    result.wasAlreadyEulerian = false;

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

    for (size_t i = 0; i + 1 < oddVertices.size(); i += 2) {
        const int u = oddVertices[i];
        const int v = oddVertices[i + 1];
        G.addEdge(u, v, 0.0);
        result.additions.push_back({u, v, "fix odd parity"});
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
