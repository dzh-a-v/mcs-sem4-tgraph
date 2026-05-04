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
    ////////// T := ∅

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
    ////////// Вход: список E рёбер графа G с длинами, упорядоченный в порядке возрастания длин

    DisjointSetUnion dsu(n);
    int edgesAdded = 0;
    const int targetEdgeCount = n - 1;
    
    int k = 0;  // индекс в массиве allEdges (0-based вместо 1-based)
    ////////// k := 1 { номер рассматриваемого ребра }

    // for i from 1 to p - 1 do
    // (добавляем ровно n-1 ребро для остовного дерева)
    while (edgesAdded < targetEdgeCount && k < static_cast<int>(allEdges.size())) {
        ////////// for i from 1 to p - 1 do
        
        // while z(T + E[k]) > 0 do
        // (проверяем, образует ли добавление ребра E[k] цикл)
        // z() > 0 означает, что ребро создаёт цикл, поэтому пропускаем его
        while (k < static_cast<int>(allEdges.size())) {
            const WeightedEdge& edge = allEdges[k];
            const int i = idToIndex.at(edge.from);
            const int j = idToIndex.at(edge.to);
            
            // dsu.unite() возвращает false, если вершины уже в одном множестве
            // (т.е. z(T + E[k]) > 0 - добавление создаст цикл)
            if (dsu.unite(i, j)) {
                break;  // цикл не образуется, можно добавить ребро
            }
            
            ++k;
            ////////// k := k + 1 { пропустить это ребро }
        }
        ////////// while z(T + E[k]) > 0 do
        
        // Если достигли конца списка рёбер, но не набрали n-1 ребро
        if (k >= static_cast<int>(allEdges.size())) {
            break;
        }
        
        const WeightedEdge& chosenEdge = allEdges[k];
        result.spanningTree->addEdge(chosenEdge.from, chosenEdge.to, chosenEdge.weight);
        result.chosenEdges.push_back(chosenEdge);
        result.totalWeight += chosenEdge.weight;
        ++edgesAdded;
        ////////// T := T + E[k] { добавить это ребро в SST }
        
        ++k;
        ////////// k := k + 1 { и исключить его из рассмотрения }
    }
    ////////// end for

    // If we added fewer than n-1 edges, the input graph was disconnected
    // and what we produced is a spanning forest, not a spanning tree.
    if (edgesAdded < targetEdgeCount) {
        result.wasConnected = false;
    }

    return result;
    ////////// Выход: множество T рёбер кратчайшего остова
}