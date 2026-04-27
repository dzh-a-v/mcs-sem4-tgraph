#pragma once

#include "include/Generator.h"
#include "include/FlowNetwork.h"
#include "include/Graph.h"

#include <memory>

// takes already generated graph and builds flow network from it
class FlowNetworkBuilder {
public:
    // graph gives us only structure (who is connected with whom)
    // capacities and costs are generated here separately from first generation
    std::unique_ptr<FlowNetwork> buildFromGraph(const AdjacencyGraph& graph);

private:
    int generateRandomCapacity();
    int generateRandomCost();

    AcyclicGraphBuilder m_generator;
};
