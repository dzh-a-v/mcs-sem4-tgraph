#include "include/FlowNetworkBuilder.h"

std::unique_ptr<FlowNetwork> FlowNetworkBuilder::buildFromGraph(const AdjacencyGraph& graph) {
    auto network = std::make_unique<FlowNetwork>(true);

    for (int vertexId : graph.vertexIds()) {
        network->addVertex(vertexId);
    }

    for (const WeightedEdge& edge : graph.edges()) {
        network->addEdge(edge.from, edge.to, generateRandomCapacity(), generateRandomCost());
    }

    return network;
}

double FlowNetworkBuilder::generateRandomCapacity() {
    const int sampled = m_generator.sampleBinomial(BINOMIAL_N_WEIGHT, BINOMIAL_P_WEIGHT);
    return sampled < 1 ? 1.0 : static_cast<double>(sampled);
}

double FlowNetworkBuilder::generateRandomCost() {
    return static_cast<double>(
        m_generator.sampleBinomial(BINOMIAL_N_WEIGHT, BINOMIAL_P_WEIGHT)
    );
}
