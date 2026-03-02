#include "include/Graph.h"
#include <algorithm>

namespace graph {

Graph::Graph(bool directed) : m_directed(directed) {}

void Graph::addVertex(int id) {
    // I don't know if we will need to add vertices with our own IDs, so I
    // added the checking.
    if (std::find(m_vertices.begin(), m_vertices.end(), id) == m_vertices.end()) {
        m_vertices.push_back(id);
    }
}

void Graph::addEdge(int from, int to, double weight) {
    // If it's directed graph, it creates one edge, otherwise two (both sides).
    addVertex(from);
    addVertex(to);
    m_adj[from].push_back({to, weight});
    if (!m_directed) {
        m_adj[to].push_back({from, weight});
    }
}

std::vector<int> Graph::vertexIds() const { return m_vertices; }

int Graph::size() const { return static_cast<int>(m_vertices.size()); }

bool Graph::isDirected() const { return m_directed; }

bool Graph::hasVertex(int id) const {
    return std::find(m_vertices.begin(), m_vertices.end(), id) != m_vertices.end();
}

std::vector<Edge> Graph::edges() const {
    std::vector<Edge> result;
    for (int u : m_vertices) { // Vertexes (IDs)
        for (auto const& [v, w] : m_adj.at(u)) { // Neighbors and weights
            if (m_directed || u < v) { // Avoiding duplicates in undirected g'.
                result.push_back({u, v, w});
            }
        }
    }
    return result;
}

std::vector<std::pair<int, double>> Graph::neighbors(int v) const {
    // also don't need in automatic generation, it's for future
    if (m_adj.find(v) == m_adj.end()) return {};
    return m_adj.at(v);
}

std::optional<double> Graph::getEdgeWeight(int from, int to) const {
    if (m_adj.find(from) == m_adj.end()) return std::nullopt; // same
    for (auto const& [v, w] : m_adj.at(from)) {
        if (v == to) return w;
    }
    return std::nullopt;
}
}