#pragma once

#include "include/FlowNetwork.h"
#include "include/Graph.h"

#include <memory>

class FlowNetworkBuilder {
public:
    std::unique_ptr<FlowNetwork> buildFromGraph(const AdjacencyGraph& graph) const;

private:
    double deriveCapacity(double weight) const;
};
