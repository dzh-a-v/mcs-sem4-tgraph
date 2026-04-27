#include "include/FlowNetworkBuilder.h"

#include <cmath>

std::unique_ptr<FlowNetwork> FlowNetworkBuilder::buildFromGraph(const AdjacencyGraph& graph) const {
    auto network = std::make_unique<FlowNetwork>(true);

    for (int vertexId : graph.vertexIds()) {
        network->addVertex(vertexId);
    }

    for (const WeightedEdge& edge : graph.edges()) {
        network->addEdge(edge.from, edge.to, deriveCapacity(edge.weight), edge.weight);
    }

    return network;
}

double FlowNetworkBuilder::deriveCapacity(double weight) const {
    const double magnitude = std::abs(weight);
    return magnitude < 1.0 ? 1.0 : magnitude;
}
