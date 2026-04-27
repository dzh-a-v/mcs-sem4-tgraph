#pragma once

#include <optional>
#include <ostream>
#include <unordered_map>
#include <vector>

struct FlowEdge {
    int from;
    int to;
    double capacity;
    double cost;
    double flow;
};

struct ResidualArc {
    int from;
    int to;
    int edgeIndex;
    bool forward;
    double residualCapacity;
    double cost;
};

using FlowMatrix = std::vector<std::vector<std::optional<double>>>;

class FlowNetwork {
public:
    explicit FlowNetwork(bool directed = true);

    void addVertex(int id);
    bool addEdge(int from, int to, double capacity, double cost);

    bool hasVertex(int id) const;
    bool isDirected() const;
    int size() const;

    std::vector<int> vertexIds() const;
    std::vector<FlowEdge> edges() const;
    std::vector<ResidualArc> residualNeighbors(int vertex) const;

    std::optional<FlowEdge> getEdge(int from, int to) const;
    std::optional<double> getCapacity(int from, int to) const;
    std::optional<double> getCost(int from, int to) const;
    std::optional<double> getFlow(int from, int to) const;

    FlowMatrix getCapacityMatrix() const;
    FlowMatrix getCostMatrix() const;
    FlowMatrix getFlowMatrix() const;

    void resetFlows();
    void augment(int edgeIndex, bool forward, double delta);

private:
    std::optional<int> findEdgeIndex(int from, int to) const;
    int indexForVertex(int id) const;

    bool m_directed;
    std::vector<int> m_vertices;
    std::unordered_map<int, int> m_idToIndex;
    std::vector<FlowEdge> m_edges;
    std::unordered_map<int, std::vector<int>> m_outgoing;
    std::unordered_map<int, std::vector<int>> m_incoming;
};

void printFlowMatrix(std::ostream& out, const FlowMatrix& matrix,
                     const std::vector<int>& vertexIds);
