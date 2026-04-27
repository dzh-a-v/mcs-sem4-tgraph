#pragma once

#include "include/Generator.h"
#include "include/FlowNetwork.h"
#include "include/Graph.h"

#include <memory>

// takes already generated graph and builds flow network from it
class FlowNetworkBuilder {
public:
    // graph gives us only structure (who is connected with whom)
    // capacities and costs are generated here separately for lab 3
    std::unique_ptr<FlowNetwork> buildFromGraph(const AdjacencyGraph& graph);

private:
    // keep cap >= 1 so edge is usable in flow network
    int generateRandomCapacity();
    // generated independently from capacity
    int generateRandomCost();

    // reuse existing generator logic from the project
    AcyclicGraphBuilder m_generator;
};
