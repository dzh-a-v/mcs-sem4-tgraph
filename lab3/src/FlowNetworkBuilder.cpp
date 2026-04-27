#include "include/FlowNetworkBuilder.h"

std::unique_ptr<FlowNetwork> FlowNetworkBuilder::buildFromGraph(const AdjacencyGraph& graph) {
    // flow network is always directed in this lab
    auto network = std::make_unique<FlowNetwork>(true);

    // copy vertex set as is
    for (int vertexId : graph.vertexIds()) {
        network->addVertex(vertexId);
    }

    // keep only topology from original graph
    // cap and cost are generated randomly here
    for (const WeightedEdge& edge : graph.edges()) {
        network->addEdge(edge.from, edge.to, generateRandomCapacity(), generateRandomCost());
    }

    return network;
}

int FlowNetworkBuilder::generateRandomCapacity() {
    // use same binomial sampler as in graph generator
    return m_generator.sampleBinomial(BINOMIAL_N_WEIGHT, BINOMIAL_P_WEIGHT);
}

int FlowNetworkBuilder::generateRandomCost() {
    // cost may be 0, that's normal
    return m_generator.sampleBinomial(BINOMIAL_N_WEIGHT, BINOMIAL_P_WEIGHT);
}
