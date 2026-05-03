#pragma once

#include "include/FlowNetwork.h"

#include <unordered_map>
#include <vector>

// one augmentation step from Ford-Fulkerson
struct MaxFlowStep {
    int iteration; // step number
    std::vector<int> path; // augmenting path by vertices
    int pathFlow; // how much was added through this path
    int totalFlow; // total flow after this step
};

struct MaxFlowResult {
    int maxFlow; // final answer
    std::vector<MaxFlowStep> steps; // useful for printing / checking
};

class MaxFlowSolver {
public:
    explicit MaxFlowSolver(FlowNetwork& network);

    // source = where flow starts
    // sink = where flow must end
    MaxFlowResult compute(int source, int sink);

private:
    // search one augmenting path in residual network
    bool bfs(int source, int sink, std::unordered_map<int, ResidualArc>& parent) const;

    FlowNetwork& m_network;
};
