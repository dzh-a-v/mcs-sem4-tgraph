#include "include/EdmondsMatching.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace {

// =====================================================================
// Internal worker class. Encapsulates all the BFS / blossom-contraction
// state for one run of the algorithm.
//
// Vertex IDs in AdjacencyGraph can be arbitrary integers, so on entry
// we densify them to [0..n). Every internal array uses these dense
// indices; we only translate back to original IDs at the very end.
// =====================================================================
class EdmondsWorker {
public:
    EdmondsWorker(const AdjacencyGraph& graph)
        : m_graph(graph)
        , m_n(graph.size())
    {
        // Build dense index <-> ID mapping.
        m_vertexIds = graph.vertexIds();
        for (int i = 0; i < m_n; ++i) {
            m_idToIndex[m_vertexIds[i]] = i;
        }

        // Build dense adjacency list for fast neighbor iteration.
        // Self-loops are skipped: they can never be part of a matching.
        m_adj.assign(m_n, {});
        for (const WeightedEdge& edge : graph.edges()) {
            const int u = m_idToIndex[edge.from];
            const int v = m_idToIndex[edge.to];
            if (u == v) continue;  // skip self-loops
            m_adj[u].push_back(v);
            m_adj[v].push_back(u);
        }
    }

    /// Main entry: run Edmonds, return match[] in DENSE indices.
    /// match[i] == -1 means vertex i is unmatched.
    std::vector<int> run() {
        m_match.assign(m_n, -1);

        // Outer loop: for each currently unmatched vertex, try to find
        // an augmenting path starting there. If found, flip it to grow M.
        // Each successful flip increases |M| by 1.
        for (int v = 0; v < m_n; ++v) {
            if (m_match[v] == -1) {
                findAugmentingPath(v);
            }
        }
        return m_match;
    }

private:
    // Graph data (in dense indices)
    const AdjacencyGraph& m_graph;
    int m_n;
    std::vector<int> m_vertexIds;          // dense index -> original ID
    std::unordered_map<int, int> m_idToIndex;  // original ID -> dense index
    std::vector<std::vector<int>> m_adj;   // dense adjacency

    // Per-augmenting-path state (rebuilt each findAugmentingPath call)
    std::vector<int> m_match;     // match[v] = partner of v in M, or -1
    std::vector<int> m_parent;    // BFS-tree parent (in the alternating forest)
    std::vector<int> m_base;      // base[v] = current "shrunk-to" representative
                                  //           of v's blossom (DSU-like, no path
                                  //           compression -- kept simple)
    std::vector<int> m_q;         // BFS queue (dense indices)
    std::vector<bool> m_inQueue;  // BFS visited flag
    std::vector<bool> m_inBlossom; // marker used when contracting a blossom

    // -----------------------------------------------------------------
    // Find the LCA of two vertices in the alternating BFS forest.
    //
    // When BFS finds an edge (a, b) between two vertices that are
    // BOTH on even levels of the SAME tree, the cycle they form with
    // their lowest common ancestor is an ODD cycle -- a blossom.
    // The LCA is the base of that blossom.
    //
    // Implementation: walk up from both a and b alternately, marking
    // visited bases. The first base we see marked from both sides is the
    // LCA. We must use base[] (not the raw parent chain) because vertices
    // already inside a previously-contracted blossom must be treated as
    // their current base for ancestry purposes.
    // -----------------------------------------------------------------
    int findLCA(int a, int b) {
        std::vector<bool> visitedByA(m_n, false);

        // Climb up from a, marking every base we pass through.
        int x = a;
        while (true) {
            x = m_base[x];
            visitedByA[x] = true;
            if (m_match[x] == -1) break;        // reached a tree root
            x = m_parent[m_match[x]];           // jump two steps up the tree
        }

        // Now climb up from b until we hit a base that A also visited.
        int y = b;
        while (true) {
            y = m_base[y];
            if (visitedByA[y]) return y;        // <-- found the LCA
            y = m_parent[m_match[y]];           // jump two steps up
        }
    }

    // -----------------------------------------------------------------
    // Mark every vertex on the cycle path from `u` up to the LCA `b`
    // as belonging to the new blossom. Also enqueue any odd-level
    // vertex that we now reach via an even path through the contracted
    // blossom (so BFS can continue past them).
    // -----------------------------------------------------------------
    void markBlossomPath(int u, int b, int child) {
        while (m_base[u] != b) {
            // Record parent so that later, when we lift the blossom to
            // reconstruct an augmenting path, we know how it was reached.
            m_parent[u] = child;
            child = m_match[u];
            // Any odd-level vertex previously dormant is now reachable
            // via an even-level path through the contracted blossom.
            if (!m_inQueue[child]) {
                m_q.push_back(child);
                m_inQueue[child] = true;
            }
            m_inBlossom[m_base[u]] = true;
            m_inBlossom[m_base[child]] = true;
            u = m_parent[child];
        }
    }

    // -----------------------------------------------------------------
    // Contract a blossom found via edge (u, v) inside the same BFS tree.
    //
    // Step 1: locate the LCA (= the base of the new blossom).
    // Step 2: walk both branches u -> LCA and v -> LCA, marking all the
    //         vertices on the cycle as "in blossom".
    // Step 3: set base[x] = LCA for every x that ended up in the blossom.
    //         This is the actual "contraction" -- from now on, any query
    //         base[x] returns the LCA, so BFS treats the whole cycle as
    //         a single super-vertex.
    // -----------------------------------------------------------------
    void contractBlossom(int u, int v) {
        const int lca = findLCA(u, v);

        std::fill(m_inBlossom.begin(), m_inBlossom.end(), false);
        markBlossomPath(u, lca, v);
        markBlossomPath(v, lca, u);

        // Every vertex whose CURRENT base ended up flagged is now in the
        // new blossom -> redirect its base to the LCA.
        for (int x = 0; x < m_n; ++x) {
            if (m_inBlossom[m_base[x]]) {
                m_base[x] = lca;
            }
        }
    }

    // -----------------------------------------------------------------
    // BFS from `root`, building an alternating forest. Returns the
    // free endpoint of an augmenting path if found, otherwise -1.
    //
    // The found path is reconstructed by walking parent[] back from the
    // endpoint to the root, then flipped (XOR-ed with M) by augment().
    // -----------------------------------------------------------------
    int bfsAugment(int root) {
        m_parent.assign(m_n, -1);
        m_inQueue.assign(m_n, false);
        m_inBlossom.assign(m_n, false);

        // Initially every vertex is its own base.
        m_base.resize(m_n);
        for (int i = 0; i < m_n; ++i) m_base[i] = i;

        m_q.clear();
        m_q.push_back(root);
        m_inQueue[root] = true;

        // BFS proper. The queue stores ONLY even-level vertices --
        // odd-level ones are reached implicitly through their match edge
        // and immediately bounce back to the even level via match[].
        for (size_t head = 0; head < m_q.size(); ++head) {
            const int u = m_q[head];
            for (int v : m_adj[u]) {
                // Skip if (u, v) is the matching edge -- it's not part of
                // the alternating structure we're exploring.
                if (m_base[u] == m_base[v] || m_match[u] == v) continue;

                // Case A: edge to root, or to a vertex whose parent we
                //         already filled in -> odd cycle inside the tree.
                //         This is a BLOSSOM, contract it.
                if (v == root || (m_match[v] != -1 && m_parent[m_match[v]] != -1)) {
                    contractBlossom(u, v);
                }
                // Case B: v hasn't been reached at all yet.
                else if (m_parent[v] == -1) {
                    m_parent[v] = u;
                    if (m_match[v] == -1) {
                        // v is FREE -> we have an augmenting path
                        // root -> ... -> u -> v.
                        return v;
                    }
                    // Otherwise, v is matched. Its match partner becomes a
                    // new even-level vertex, push to queue and continue BFS.
                    m_q.push_back(m_match[v]);
                    m_inQueue[m_match[v]] = true;
                }
            }
        }
        return -1;  // no augmenting path from `root`
    }

    // -----------------------------------------------------------------
    // Augment along the path ending at `v` (a free vertex). Walk parents
    // back to the root, flipping each pair (parent -> match) along the way.
    // Net effect on M: |M| grows by 1, the path's free endpoints become
    // matched, and matching edges along the path get "rotated" by one step.
    // -----------------------------------------------------------------
    void augment(int v) {
        while (v != -1) {
            const int pv = m_parent[v];
            const int next = (pv == -1) ? -1 : m_match[pv];
            // Make (pv, v) a matching edge.
            m_match[v] = pv;
            m_match[pv] = v;
            v = next;
        }
    }

    // -----------------------------------------------------------------
    // Combine BFS + augment: search from `root`, and if an augmenting
    // path was found, immediately flip it.
    // -----------------------------------------------------------------
    void findAugmentingPath(int root) {
        const int endpoint = bfsAugment(root);
        if (endpoint != -1) {
            augment(endpoint);
        }
    }
};

}  // namespace

// =====================================================================
// Public driver. Runs the algorithm, then converts the dense match[]
// array into the MatchingResult format that GreedyMaximalMatching also
// uses, so main.cpp doesn't care which algorithm produced the result.
// =====================================================================
EdmondsMatching::EdmondsMatching(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

MatchingResult EdmondsMatching::compute() const {
    MatchingResult result;
    result.matchingSize = 0;
    result.isPerfect = false;

    const int n = m_graph.size();
    if (n == 0) {
        return result;
    }

    EdmondsWorker worker(m_graph);
    const std::vector<int> match = worker.run();  // match[] in dense indices

    // Translate dense matches back into (original-ID) WeightedEdge entries.
    // Each matched pair (i, match[i]) appears twice in match[] -- once from
    // each endpoint -- so we report it only when i < match[i] to avoid dupes.
    const std::vector<int> vertexIds = m_graph.vertexIds();
    std::unordered_map<int, int> idToIndex;
    for (int i = 0; i < n; ++i) idToIndex[vertexIds[i]] = i;

    for (int i = 0; i < n; ++i) {
        const int j = match[i];
        if (j == -1 || j < i) continue;  // unmatched, or duplicate direction

        const int idA = vertexIds[i];
        const int idB = vertexIds[j];

        // Look up the original edge weight for nicer output.
        // The matching itself didn't use weights, but the user sees them.
        auto weightOpt = m_graph.getEdgeWeight(idA, idB);
        if (!weightOpt.has_value()) {
            // Undirected: try the reverse direction too.
            weightOpt = m_graph.getEdgeWeight(idB, idA);
        }
        const double w = weightOpt.value_or(0.0);

        result.matchingEdges.push_back({idA, idB, w});
    }

    // Sort matching edges lexicographically for deterministic, reproducible output.
    std::sort(result.matchingEdges.begin(), result.matchingEdges.end(),
        [](const WeightedEdge& a, const WeightedEdge& b) {
            if (a.from != b.from) return a.from < b.from;
            return a.to < b.to;
        });

    result.matchingSize = static_cast<int>(result.matchingEdges.size());

    // Collect unmatched original IDs.
    for (int i = 0; i < n; ++i) {
        if (match[i] == -1) {
            result.unmatchedVertices.push_back(vertexIds[i]);
        }
    }
    result.isPerfect = result.unmatchedVertices.empty();

    return result;
}
