#pragma once

#include "include/Graph.h"

#include <filesystem>

class MermaidExporter {
public:
    static std::filesystem::path exportGraph(const AdjacencyGraph& graph,
                                             const std::string& fileName = "generated_graph.mmd");

private:
    static std::filesystem::path executableDirectory();
};
