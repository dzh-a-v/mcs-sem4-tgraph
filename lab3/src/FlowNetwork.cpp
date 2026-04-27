#include "include/FlowNetwork.h"

#include <cmath>
#include <iomanip>

namespace {
constexpr double kEps = 1e-9;

FlowMatrix createEmptyMatrix(int size) {
    return FlowMatrix(size, std::vector<std::optional<double>>(size, std::nullopt));
}
}

FlowNetwork::FlowNetwork(bool directed)
    : m_directed(directed) {}

void FlowNetwork::addVertex(int id) {
    if (hasVertex(id)) {
        return;
    }

    m_idToIndex[id] = static_cast<int>(m_vertices.size());
    m_vertices.push_back(id);
}

bool FlowNetwork::addEdge(int from, int to, double capacity, double cost) {
    if (capacity < 0.0 || findEdgeIndex(from, to).has_value()) {
        return false;
    }

    addVertex(from);
    addVertex(to);

    const int edgeIndex = static_cast<int>(m_edges.size());
    m_edges.push_back({from, to, capacity, cost, 0.0});
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

    auto outIt = m_outgoing.find(vertex);
    if (outIt != m_outgoing.end()) {
        for (int edgeIndex : outIt->second) {
            const FlowEdge& edge = m_edges[edgeIndex];
            const double residualCapacity = edge.capacity - edge.flow;
            if (residualCapacity > kEps) {
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

    auto inIt = m_incoming.find(vertex);
    if (inIt != m_incoming.end()) {
        for (int edgeIndex : inIt->second) {
            const FlowEdge& edge = m_edges[edgeIndex];
            if (edge.flow > kEps) {
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

    return m_edges[edgeIndex.value()];
}

std::optional<double> FlowNetwork::getCapacity(int from, int to) const {
    auto edge = getEdge(from, to);
    if (!edge.has_value()) {
        return std::nullopt;
    }
    return edge->capacity;
}

std::optional<double> FlowNetwork::getCost(int from, int to) const {
    auto edge = getEdge(from, to);
    if (!edge.has_value()) {
        return std::nullopt;
    }
    return edge->cost;
}

std::optional<double> FlowNetwork::getFlow(int from, int to) const {
    auto edge = getEdge(from, to);
    if (!edge.has_value()) {
        return std::nullopt;
    }
    return edge->flow;
}

FlowMatrix FlowNetwork::getCapacityMatrix() const {
    FlowMatrix matrix = createEmptyMatrix(size());
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
    for (FlowEdge& edge : m_edges) {
        edge.flow = 0.0;
    }
}

void FlowNetwork::augment(int edgeIndex, bool forward, double delta) {
    if (edgeIndex < 0 || edgeIndex >= static_cast<int>(m_edges.size())) {
        return;
    }

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
                double value = matrix[row][col].value();
                if (std::abs(value) < kEps) {
                    value = 0.0;
                }
                out << std::setw(10) << value;
            } else {
                out << std::setw(10) << "i";
            }
        }
        out << "\n";
    }
}
