#pragma once

#include "include/FlowNetwork.h"

#include <unordered_map>
#include <vector>

// one augmentation step for min-cost-flow
struct MinCostFlowStep {
    int iteration; // step number
    std::vector<int> path; // chosen shortest path in residual network
    int pathFlow; // how much flow was added through this path
    int pathUnitCost; // cost of 1 unit along this path
    int cumulativeFlow; // total sent flow after this step
    int cumulativeCost; // total cost after this step
};

struct MinCostFlowResult {
    int targetFlow; // what we wanted to send
    int achievedFlow; // what was actually sent
    int totalCost; // final price
    bool success; // was target reached fully?
    std::vector<MinCostFlowStep> steps; // for printing / checking
};

class MinCostFlowSolver {
public:
    explicit MinCostFlowSolver(FlowNetwork& network);

    // sends exactly targetFlow if possible, but with minimal cost
    MinCostFlowResult compute(int source, int sink, int targetFlow);

private:
    // local dijkstra on residual network
    // still follows the same logic as old lab dijkstra
    bool dijkstra(int source, int sink,
                  std::unordered_map<int, int>& distances,
                  std::unordered_map<int, ResidualArc>& parent) const;

    FlowNetwork& m_network;
};
