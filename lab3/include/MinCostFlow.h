#pragma once

#include "include/FlowNetwork.h"

#include <unordered_map>
#include <vector>

struct MinCostFlowStep {
    int iteration;
    std::vector<int> path;
    double pathFlow;
    double pathUnitCost;
    double cumulativeFlow;
    double cumulativeCost;
};

struct MinCostFlowResult {
    double targetFlow;
    double achievedFlow;
    double totalCost;
    bool success;
    std::vector<MinCostFlowStep> steps;
};

class MinCostFlowSolver {
public:
    explicit MinCostFlowSolver(FlowNetwork& network);

    MinCostFlowResult compute(int source, int sink, double targetFlow);

private:
    bool dijkstra(int source, int sink,
                  std::unordered_map<int, double>& distances,
                  std::unordered_map<int, ResidualArc>& parent) const;

    FlowNetwork& m_network;
};
