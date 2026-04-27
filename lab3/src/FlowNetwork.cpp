#include "include/FlowNetwork.h"

#include <iomanip>

namespace {
// helper so all 3 matrix builders use the same empty "square" (filled w/ nullopt)
FlowMatrix createEmptyMatrix(int size) {
    return FlowMatrix(size, std::vector<std::optional<int>>(size, std::nullopt));
}
}

FlowNetwork::FlowNetwork(bool directed)
    : m_directed(directed) {}

void FlowNetwork::addVertex(int id) {
    if (hasVertex(id)) {
        return;
    }

    // index is just current position in vertex list
    m_idToIndex[id] = static_cast<int>(m_vertices.size());
    m_vertices.push_back(id);
}

bool FlowNetwork::addEdge(int from, int to, int capacity, int cost) {
    // no negative capacities and no duplicate directed edge
    if (capacity < 0 || findEdgeIndex(from, to).has_value()) {
        return false;
    }

    // add endpoints if needed
    addVertex(from);
    addVertex(to);

    // flow starts from 0, algorithms will change it later
    const int edgeIndex = static_cast<int>(m_edges.size());
    m_edges.push_back({from, to, capacity, cost, 0});

    // store only indexes into m_edges, so we don't duplicate the edge itself
    m_outgoing[from].push_back(edgeIndex);
    m_incoming[to].push_back(edgeIndex);
    return true;
}

bool FlowNetwork::hasVertex(int id) const {
    return m_idToIndex.find(id) != m_idToIndex.end();
}

bool FlowNetwork::isDirected() const {
    return m_directed;
}

int FlowNetwork::size() const {
    return static_cast<int>(m_vertices.size());
}

std::vector<int> FlowNetwork::vertexIds() const {
    return m_vertices;
}

std::vector<FlowEdge> FlowNetwork::edges() const {
    return m_edges;
}

std::vector<ResidualArc> FlowNetwork::residualNeighbors(int vertex) const {
    std::vector<ResidualArc> residuals;

    // 1) forward residual edges:
    // if edge still has free capacity, we can push more flow forward
    auto outIt = m_outgoing.find(vertex);
    if (outIt != m_outgoing.end()) {
        for (int edgeIndex : outIt->second) {
            const FlowEdge& edge = m_edges[edgeIndex];
            const int residualCapacity = edge.capacity - edge.flow;
            if (residualCapacity > 0) {
                residuals.push_back({
                    edge.from,
                    edge.to,
                    edgeIndex,
                    true,
                    residualCapacity,
                    edge.cost
                });
            }
        }
    }

    // 2) backward residual edges:
    // if some flow was already pushed, we may "take it back"
    auto inIt = m_incoming.find(vertex);
    if (inIt != m_incoming.end()) {
        for (int edgeIndex : inIt->second) {
            const FlowEdge& edge = m_edges[edgeIndex];
            if (edge.flow > 0) {
                residuals.push_back({
                    edge.to,
                    edge.from,
                    edgeIndex,
                    false,
                    edge.flow,
                    -edge.cost
                });
            }
        }
    }

    return residuals;
}

std::optional<FlowEdge> FlowNetwork::getEdge(int from, int to) const {
    auto edgeIndex = findEdgeIndex(from, to);
    if (!edgeIndex.has_value()) {
        return std::nullopt;
    }

    // return a copy, not reference
    return m_edges[edgeIndex.value()];
}

std::optional<int> FlowNetwork::getCapacity(int from, int to) const {
    auto edge = getEdge(from, to);
    if (!edge.has_value()) {
        return std::nullopt;
    }
    return edge->capacity;
}

std::optional<int> FlowNetwork::getCost(int from, int to) const {
    auto edge = getEdge(from, to);
    if (!edge.has_value()) {
        return std::nullopt;
    }
    return edge->cost;
}

std::optional<int> FlowNetwork::getFlow(int from, int to) const {
    auto edge = getEdge(from, to);
    if (!edge.has_value()) {
        return std::nullopt;
    }
    return edge->flow;
}

FlowMatrix FlowNetwork::getCapacityMatrix() const {
    FlowMatrix matrix = createEmptyMatrix(size());

    // only real edges are written here
    for (const FlowEdge& edge : m_edges) {
        matrix[indexForVertex(edge.from)][indexForVertex(edge.to)] = edge.capacity;
    }
    return matrix;
}

FlowMatrix FlowNetwork::getCostMatrix() const {
    FlowMatrix matrix = createEmptyMatrix(size());

    for (const FlowEdge& edge : m_edges) {
        matrix[indexForVertex(edge.from)][indexForVertex(edge.to)] = edge.cost;
    }
    return matrix;
}

FlowMatrix FlowNetwork::getFlowMatrix() const {
    FlowMatrix matrix = createEmptyMatrix(size());

    for (const FlowEdge& edge : m_edges) {
        matrix[indexForVertex(edge.from)][indexForVertex(edge.to)] = edge.flow;
    }
    return matrix;
}

void FlowNetwork::resetFlows() {
    // keep graph / capacities / costs, only clear current flow state
    for (FlowEdge& edge : m_edges) {
        edge.flow = 0;
    }
}

void FlowNetwork::augment(int edgeIndex, bool forward, int delta) {
    // safety check, just ignore bad index
    if (edgeIndex < 0 || edgeIndex >= static_cast<int>(m_edges.size())) {
        return;
    }

    // forward residual move => add flow
    // backward residual move => reduce previously pushed flow
    if (forward) {
        m_edges[edgeIndex].flow += delta;
    } else {
        m_edges[edgeIndex].flow -= delta;
    }
}

std::optional<int> FlowNetwork::findEdgeIndex(int from, int to) const {
    auto outIt = m_outgoing.find(from);
    if (outIt == m_outgoing.end()) {
        return std::nullopt;
    }

    // scan only outgoing edges from "from", not all edges in graph
    for (int edgeIndex : outIt->second) {
        const FlowEdge& edge = m_edges[edgeIndex];
        if (edge.to == to) {
            return edgeIndex;
        }
    }

    return std::nullopt;
}

int FlowNetwork::indexForVertex(int id) const {
    return m_idToIndex.at(id);
}

void printFlowMatrix(std::ostream& out, const FlowMatrix& matrix,
                     const std::vector<int>& vertexIds) {
    const size_t n = matrix.size();
    if (n == 0) {
        return;
    }

    out << "      ";
    for (int vertexId : vertexIds) {
        out << std::setw(10) << vertexId;
    }
    out << "\n";

    out << "      ";
    for (size_t i = 0; i < n; ++i) {
        out << "----------";
    }
    out << "\n";

    for (size_t row = 0; row < n; ++row) {
        out << std::setw(5) << vertexIds[row] << " |";
        for (size_t col = 0; col < n; ++col) {
            if (matrix[row][col].has_value()) {
                out << std::setw(10) << matrix[row][col].value();
            } else {
                // same convention as in other parts of the project
                out << std::setw(10) << "i";
            }
        }
        out << "\n";
    }
}
