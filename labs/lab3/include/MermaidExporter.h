#pragma once

#include "include/FlowNetwork.h"
#include "include/Graph.h"

#include <filesystem>

class MermaidExporter {
public:
    static std::filesystem::path exportGraph(const AdjacencyGraph& graph,
                                             const std::string& fileName = "generated_graph.mmd");
    static std::filesystem::path exportFlowNetwork(
        const FlowNetwork& network,
        const std::string& fileName = "generated_flow_network.mmd");

private:
    static std::filesystem::path executableDirectory();
};
