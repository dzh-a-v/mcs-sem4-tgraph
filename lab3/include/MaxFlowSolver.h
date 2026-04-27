#pragma once

#include "include/FlowNetwork.h"

#include <unordered_map>
#include <vector>

struct MaxFlowStep {
    int iteration;
    std::vector<int> path;
    int pathFlow;
    int totalFlow;
};

struct MaxFlowResult {
    int maxFlow;
    std::vector<MaxFlowStep> steps;
};

class MaxFlowSolver {
public:
    explicit MaxFlowSolver(FlowNetwork& network);

    MaxFlowResult compute(int source, int sink);

private:
    bool bfs(int source, int sink, std::unordered_map<int, ResidualArc>& parent) const;

    FlowNetwork& m_network;
};
