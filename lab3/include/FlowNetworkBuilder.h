#pragma once

#include "include/Generator.h"
#include "include/FlowNetwork.h"
#include "include/Graph.h"

#include <memory>

class FlowNetworkBuilder {
public:
    std::unique_ptr<FlowNetwork> buildFromGraph(const AdjacencyGraph& graph);

private:
    int generateRandomCapacity();
    int generateRandomCost();

    AcyclicGraphBuilder m_generator;
};
