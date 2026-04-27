#pragma once

#include "include/FlowNetwork.h"

#include <unordered_map>
#include <vector>

struct MinCostFlowStep {
    int iteration;
    std::vector<int> path;
    int pathFlow;
    int pathUnitCost;
    int cumulativeFlow;
    int cumulativeCost;
};

struct MinCostFlowResult {
    int targetFlow;
    int achievedFlow;
    int totalCost;
    bool success;
    std::vector<MinCostFlowStep> steps;
};

class MinCostFlowSolver {
public:
    explicit MinCostFlowSolver(FlowNetwork& network);

    MinCostFlowResult compute(int source, int sink, int targetFlow);

private:
    bool dijkstra(int source, int sink,
                  std::unordered_map<int, int>& distances,
                  std::unordered_map<int, ResidualArc>& parent) const;

    FlowNetwork& m_network;
};
