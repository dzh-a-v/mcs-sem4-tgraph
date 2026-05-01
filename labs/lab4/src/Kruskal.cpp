#include "include/Kruskal.h"
#include <algorithm>
#include <unordered_map>

namespace {

// Disjoint-Set Union with union-by-rank and path compression.
// Operations are amortized O(alpha(n)), effectively constant.
// Used by Kruskal to detect whether adding an edge would create a cycle.
class DisjointSetUnion {
public:
    explicit DisjointSetUnion(int size)
        : m_parent(size)
        , m_rank(size, 0)
    {
        // Initially every element is its own parent (n singleton sets).
        for (int i = 0; i < size; ++i) {
            m_parent[i] = i;
        }
    }

    // Find the representative of the set containing x, with path compression.
    int find(int x) {
        // Iterative path compression: first walk up to the root,
        // then make every node on the path point directly to it.
        int root = x;
        while (m_parent[root] != root) {
            root = m_parent[root];
        }
        while (m_parent[x] != root) {
            const int next = m_parent[x];
            m_parent[x] = root;
            x = next;
        }
        return root;
    }

    // Union the sets containing x and y. Returns true if they were in
    // different sets (i.e. the union actually happened), false otherwise.
    // Uses union by rank to keep the tree shallow.
    bool unite(int x, int y) {
        int rx = find(x);
        int ry = find(y);
        if (rx == ry) {
            return false;  // already in the same set -- adding edge would create a cycle
        }

        // Attach the shorter tree under the taller one.
        if (m_rank[rx] < m_rank[ry]) {
            m_parent[rx] = ry;
        } else if (m_rank[rx] > m_rank[ry]) {
            m_parent[ry] = rx;
        } else {
            m_parent[ry] = rx;
            ++m_rank[rx];
        }
        return true;
    }

private:
    std::vector<int> m_parent;
    std::vector<int> m_rank;
};

}  // namespace

KruskalMST::KruskalMST(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

KruskalResult KruskalMST::buildMST() const {
    KruskalResult result;
    result.totalWeight = 0.0;
    result.wasConnected = true;

    const int n = m_graph.size();

    // Output tree inherits directedness from the input graph.
    // For Lab 4 the user always passes an undirected graph, so this
    // is effectively always undirected.
    result.spanningTree = std::make_unique<AdjacencyGraph>(m_graph.isDirected());

    // Always add all vertices, even if some end up isolated (disconnected case).
    for (int v : m_graph.vertexIds()) {
        result.spanningTree->addVertex(v);
    }

    if (n <= 1) {
        // Empty graph or single vertex: nothing to connect.
        return result;
    }

    // Map vertex IDs to dense [0..n) indices for the DSU.
    auto vertexIds = m_graph.vertexIds();
    std::unordered_map<int, int> idToIndex;
    for (int i = 0; i < n; ++i) {
        idToIndex[vertexIds[i]] = i;
    }

    // Collect all edges. AdjacencyGraph::edges() already yields each
    // undirected edge exactly once (filtered by u < v internally).
    std::vector<WeightedEdge> allEdges = m_graph.edges();

    // Sort by ascending weight. Tie-break by (from, to) for deterministic output:
    // running Kruskal twice on the same graph should produce the same MST.
    std::sort(allEdges.begin(), allEdges.end(),
        [](const WeightedEdge& a, const WeightedEdge& b) {
            if (a.weight != b.weight) {
                return a.weight < b.weight;
            }
            if (a.from != b.from) {
                return a.from < b.from;
            }
            return a.to < b.to;
        });

    DisjointSetUnion dsu(n);
    int edgesAdded = 0;
    const int targetEdgeCount = n - 1;

    // Process edges in ascending weight order.
    // We could break out as soon as edgesAdded == targetEdgeCount,
    // but iterating to the end is harmless and keeps the code simple.
    for (const WeightedEdge& edge : allEdges) {
        if (edgesAdded == targetEdgeCount) {
            break;  // tree is complete
        }

        const int i = idToIndex.at(edge.from);
        const int j = idToIndex.at(edge.to);

        // unite() returns true if the endpoints were in different components,
        // i.e. adding this edge would NOT create a cycle.
        if (dsu.unite(i, j)) {
            result.spanningTree->addEdge(edge.from, edge.to, edge.weight);
            result.chosenEdges.push_back(edge);
            result.totalWeight += edge.weight;
            ++edgesAdded;
        }
    }

    // If we added fewer than n-1 edges, the input graph was disconnected
    // and what we produced is a spanning forest, not a spanning tree.
    if (edgesAdded < targetEdgeCount) {
        result.wasConnected = false;
    }

    return result;
}
