#include "include/Graph.h"
#include <algorithm>

AdjacencyGraph::AdjacencyGraph(bool directed) : m_directed(directed) {}

void AdjacencyGraph::addVertex(int id) {
    if (std::find(m_vertices.begin(), m_vertices.end(), id) == m_vertices.end()) {
        m_vertices.push_back(id);
    }
}

void AdjacencyGraph::addEdge(int from, int to, double weight) {
    addVertex(from);
    addVertex(to);
    m_adj[from].push_back({to, weight});
    if (!m_directed) {
        m_adj[to].push_back({from, weight});
    }
}

std::vector<int> AdjacencyGraph::vertexIds() const { 
    return m_vertices; 
}

int AdjacencyGraph::size() const { 
    return static_cast<int>(m_vertices.size()); 
}

bool AdjacencyGraph::isDirected() const { 
    return m_directed; 
}

bool AdjacencyGraph::hasVertex(int id) const {
    return std::find(m_vertices.begin(), m_vertices.end(), id) != m_vertices.end();
}

std::vector<WeightedEdge> AdjacencyGraph::edges() const {
    std::vector<WeightedEdge> edgeList;
    for (int u : m_vertices) {
        auto it = m_adj.find(u);
        if (it != m_adj.end()) {
            for (auto const& [v, w] : it->second) {
                if (m_directed || u < v) {
                    edgeList.push_back({u, v, w});
                }
            }
        }
    }
    return edgeList;
}

std::vector<std::pair<int, double>> AdjacencyGraph::neighbors(int v) const {
    if (m_adj.find(v) == m_adj.end()) return {};
    return m_adj.at(v);
}

std::optional<double> AdjacencyGraph::getEdgeWeight(int from, int to) const {
    if (m_adj.find(from) == m_adj.end()) return std::nullopt;
    for (auto const& [v, w] : m_adj.at(from)) {
        if (v == to) return w;
    }
    return std::nullopt;
}
