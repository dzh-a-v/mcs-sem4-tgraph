#include "include/MermaidExporter.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <windows.h>

namespace {
std::string formatWeight(double weight) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << weight;
    std::string text = out.str();

    while (!text.empty() && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }
    if (text.empty()) {
        return "0";
    }
    return text;
}
}

std::filesystem::path MermaidExporter::exportGraph(const AdjacencyGraph& graph,
                                                   const std::string& fileName) {
    const std::filesystem::path outputPath = executableDirectory() / fileName;
    std::ofstream out(outputPath);
    if (!out) {
        throw std::runtime_error("Failed to create Mermaid file");
    }

    out << "graph LR\n";

    for (int vertexId : graph.vertexIds()) {
        out << "    v" << vertexId << "[\"" << vertexId << "\"]\n";
    }

    const std::string connector = graph.isDirected() ? "-->" : "---";
    for (const WeightedEdge& edge : graph.edges()) {
        out << "    v" << edge.from
            << " " << connector << "|\"" << formatWeight(edge.weight) << "\"| "
            << "v" << edge.to << "\n";
    }

    return outputPath;
}

std::filesystem::path MermaidExporter::executableDirectory() {
    char buffer[MAX_PATH];
    const DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (length == 0 || length == MAX_PATH) {
        throw std::runtime_error("Failed to resolve executable path");
    }

    return std::filesystem::path(buffer).parent_path();
}
