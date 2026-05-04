#include <iostream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <cctype>
#include <string>
#include <tuple>
#include <vector>
#include <algorithm>
#include "include/Graph.h"
#include "include/Generator.h"
#include "include/ShimbellMethod.h"
#include "include/PathCounter.h"
#include "include/Eccentricity.h"
#include "include/BFS.h"
#include "include/Dijkstra.h"
#include "include/AlgorithmComparator.h"
#include "include/FlowNetwork.h"
#include "include/FlowNetworkBuilder.h"
#include "include/MaxFlowSolver.h"
#include "include/MinCostFlow.h"
#include "include/MermaidExporter.h"
#include "include/Kirchhoff.h"
#include "include/Kruskal.h"
#include "include/PruferCode.h"
#include "include/MaxMatching.h"
#include "include/EulerianCycle.h"
#include "include/FundamentalCuts.h"

// ============================================================================
// Helper Functions
// ============================================================================

void showMenu(bool hideLab1, bool hideLab2, bool hideLab3, bool hideLab4, bool hideLab5, bool hideCustom) {
    std::cout << "\n========== MAIN MENU ==========\n";
    if (!hideLab1) {
        std::cout << "\n  --- Lab 1 ---\n";
        std::cout << "  1. Generate random graph\n";
        std::cout << "  2. Calculate eccentricities & center\n";
        std::cout << "  3. Shimbell's method (k-edge paths)\n";
        std::cout << "  4. Find all paths between vertices\n";
        std::cout << "  5. Show graph details\n";
        std::cout << "  6. Compare distribution statistics\n";
        std::cout << "  7. Show adjacency matrix\n";
        std::cout << "  8. Show weight matrix (Shimbell k=1)\n";
    }
    if (!hideLab2) {
        std::cout << "\n  --- Lab 2 ---\n";
        std::cout << "  9. BFS traversal\n";
        std::cout << "  10. Dijkstra's shortest path\n";
        std::cout << "  11. Compare BFS vs Dijkstra (speed)\n";
    }
    if (!hideLab3) {
        std::cout << "\n  --- Lab 3 ---\n";
        std::cout << "  12. Build flow network from current directed graph\n";
        std::cout << "  13. Show capacity matrix\n";
        std::cout << "  14. Show cost matrix\n";
        std::cout << "  15. Find maximum flow\n";
        std::cout << "  16. Find minimum-cost flow for [2/3 * max]\n";
        std::cout << "  17. Show flow network details\n";
    }  
    if (!hideLab4) {  
        std::cout << "\n  --- Lab 4 ---\n";
        std::cout << "  18. Count spanning trees (Kirchhoff's theorem)\n";
        std::cout << "  19. Build minimum spanning tree (Kruskal)\n";
        std::cout << "  20. Show spanning tree details\n";
        std::cout << "  21. Encode spanning tree to Prufer code\n";
        std::cout << "  22. Decode last Prufer code back to a tree\n";
        std::cout << "  23. Maximal matching (independent edge set)\n";
    }
    if (!hideLab5) {
        std::cout << "\n  --- Lab 5 ---\n";
        std::cout << "  24. Build Eulerian cycle (modify graph if needed)\n";
        std::cout << "  25. Show fundamental cut-set system (uses MST)\n";
        std::cout << "  26. Combine cuts via symmetric difference\n";
    }
    if (!hideCustom) {
        std::cout << "\n  --- Custom ---\n";
        std::cout << "  1001. Toggle hiding Lab 1 in menu\n";
        std::cout << "  1002. Toggle hiding Lab 2 in menu\n";
        std::cout << "  1003. Toggle hiding Lab 3 in menu\n";
        std::cout << "  1004. Toggle hiding Lab 4 in menu\n";
        std::cout << "  1005. Toggle hiding Lab 5 in menu\n";
        std::cout << "  1006. Toggle hiding Custom in menu\n";
        std::cout << "  101. Export current graph to Mermaid\n";
        std::cout << "  102. Export current flow network to Mermaid\n";
        std::cout << "  103. Export current spanning tree to Mermaid\n";
        std::cout << "  0. Quit\n";
        std::cout << "===============================\n";
    }
    std::cout << "> ";
}

int readInteger(const std::string& prompt) {
    std::cout << prompt;
    int value;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. " << prompt;
    }
    return value;
}

int readNonNegativeIntegerStrict(const std::string& prompt, const std::string& errorMessage) {
    std::cout << prompt;

    while (true) {
        std::string token;
        if (!(std::cin >> token)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << errorMessage;
            continue;
        }

        bool allDigits = !token.empty();
        for (unsigned char ch : token) {
            if (!std::isdigit(ch)) {
                allDigits = false;
                break;
            }
        }

        if (!allDigits) {
            std::cout << errorMessage;
            continue;
        }

        try {
            return std::stoi(token);
        } catch (const std::exception&) {
            std::cout << errorMessage;
        }
    }
}

WeightSign selectWeightSign() {
    std::cout << "\n>>> Choose weight distribution:\n";
    std::cout << "  1. Positive values only\n";
    std::cout << "  2. Negative values only\n";
    std::cout << "  3. Mixed (positive and negative)\n";
    std::cout << "> ";
    
    int option;
    while (!(std::cin >> option) || option < 1 || option > 3) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid. Enter 1-3: ";
    }
    
    switch (option) {
        case 1: return WeightSign::Positive;
        case 2: return WeightSign::Negative;
        case 3: return WeightSign::Mixed;
        default: return WeightSign::Positive;
    }
}

void displayMatrix(const std::string& title, const WeightTable& matrix, const std::vector<int>& vertexIds) {
    std::cout << "\n--- " << title << " ---\n";
    std::cout << "i = no path\n\n";
    
    // Header row
    std::cout << "      ";
    for (int id : vertexIds) {
        std::cout << std::setw(10) << id;
    }
    std::cout << "\n";

    // Separator
    std::cout << "      ";
    for (size_t i = 0; i < matrix.size(); ++i) {
        std::cout << "----------";
    }
    std::cout << "\n";

    // Data rows
    for (size_t row = 0; row < matrix.size(); ++row) {
        std::cout << std::setw(5) << vertexIds[row] << " |";
        for (size_t col = 0; col < matrix[row].size(); ++col) {
            if (matrix[row][col].has_value()) {
                std::cout << std::setw(10) << matrix[row][col].value();
            } else {
                std::cout << std::setw(10) << "i";
            }
        }
        std::cout << "\n";
    }
}

void showDistributionInfo() {
    std::cout << "\n=== BINOMIAL DISTRIBUTION PARAMETERS ===\n";
    
    auto props = AcyclicGraphBuilder::getBinomialProperties(BINOMIAL_N_DEGREE, BINOMIAL_P_DEGREE);
    
    std::cout << "For degrees B(" << BINOMIAL_N_DEGREE << ", " << BINOMIAL_P_DEGREE << "):\n";
    std::cout << "  Expected mean:   " << props.mean << "\n";
    std::cout << "  Expected var:    " << props.variance << "\n";
    std::cout << "  Expected std:    " << props.stdDev << "\n";
    std::cout << "  Mode:            " << props.mode << "\n";
    
    std::cout << "\nFor weights B(" << BINOMIAL_N_WEIGHT << ", " << BINOMIAL_P_WEIGHT << "):\n";
    std::cout << "  Expected mean:   " << (BINOMIAL_N_WEIGHT * BINOMIAL_P_WEIGHT) << "\n";
    std::cout << "  Expected var:    " << (BINOMIAL_N_WEIGHT * BINOMIAL_P_WEIGHT * (1.0 - BINOMIAL_P_WEIGHT)) << "\n";
}

void showGraphStats(const std::unique_ptr<AdjacencyGraph>& graph) {
    if (!graph) {
        std::cout << "[!] No graph available\n";
        return;
    }
    
    std::cout << "\n=== ACTUAL GRAPH STATISTICS ===\n";
    auto stats = AcyclicGraphBuilder::computeDegreeStatistics(*graph);
    
    std::cout << "Vertices:       " << graph->size() << "\n";
    std::cout << "Edges:          " << graph->edges().size() << "\n";
    std::cout << "Mean degree:    " << stats.meanDegree << "\n";
    std::cout << "Variance:       " << stats.variance << "\n";
    std::cout << "Std deviation:  " << stats.stdDev << "\n";
    std::cout << "Min degree:     " << stats.minDegree << "\n";
    std::cout << "Max degree:     " << stats.maxDegree << "\n";
    
    // Compare with theory
    auto props = AcyclicGraphBuilder::getBinomialProperties(BINOMIAL_N_DEGREE, BINOMIAL_P_DEGREE);
    std::cout << "\n--- Deviation from Theory ---\n";
    std::cout << "Mean error:  " << std::abs(stats.meanDegree - props.mean) << "\n";
    std::cout << "Var error:   " << std::abs(stats.variance - props.variance) << "\n";
}

void showGraphDetails(const std::unique_ptr<AdjacencyGraph>& graph) {
    if (!graph) {
        std::cout << "[!] No graph available\n";
        return;
    }
    
    std::cout << "\n=== GRAPH INFORMATION ===\n";
    std::cout << "Type:       " << (graph->isDirected() ? "Directed" : "Undirected") << "\n";
    std::cout << "Vertices:   " << graph->size() << "\n";
    std::cout << "Edges:      " << graph->edges().size() << "\n";
    std::cout << "Vertex IDs: ";
    for (int id : graph->vertexIds()) {
        std::cout << id << " ";
    }
    std::cout << "\n";
}

void printVertexPath(const std::vector<int>& path) {
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << "v" << path[i];
        if (i + 1 < path.size()) {
            std::cout << " -> ";
        }
    }
}

void showFlowNetworkDetails(const std::unique_ptr<FlowNetwork>& network) {
    if (!network) {
        std::cout << "[!] No flow network available\n";
        return;
    }

    std::cout << "\n=== FLOW NETWORK ===\n";
    std::cout << "Type:       " << (network->isDirected() ? "Directed" : "Undirected") << "\n";
    std::cout << "Vertices:   " << network->size() << "\n";
    std::cout << "Edges:      " << network->edges().size() << "\n";
    std::cout << "Vertex IDs: ";
    for (int vertexId : network->vertexIds()) {
        std::cout << vertexId << " ";
    }
    std::cout << "\n";

    std::cout << "\nEdges:\n";
    for (const FlowEdge& edge : network->edges()) {
        std::cout << "  v" << edge.from << " -> v" << edge.to
                  << " | capacity = " << edge.capacity
                  << ", cost = " << edge.cost
                  << ", flow = " << edge.flow << "\n";
    }
}

bool sinkHasOnlyZeroCapacityIncomingEdges(const FlowNetwork& network, int sink) {
    bool hasIncomingEdges = false;

    for (const FlowEdge& edge : network.edges()) {
        if (edge.to == sink) {
            hasIncomingEdges = true;
            if (edge.capacity > 0) {
                return false;
            }
        }
    }

    return hasIncomingEdges;
}

// ============================================================================
// Main Program
// ============================================================================

int main() {
    std::unique_ptr<AdjacencyGraph> currentGraph;
    std::unique_ptr<FlowNetwork> currentFlowNetwork;
    std::unique_ptr<AdjacencyGraph> currentSpanningTree;
    PruferCode lastPruferCode;
    bool hasPruferCode = false;
    std::vector<FundamentalCut> lastFundamentalCuts;  // cached for option 26
    int userChoice;
    int lastMaxFlow = 0;
    int lastFlowSource = -1;
    int lastFlowSink = -1;
    bool hasMaxFlowResult = false;
    bool hideLab1 = false;
    bool hideLab2 = false;
    bool hideLab3 = false;
    bool hideLab4 = false;
    bool hideLab5 = false;
    bool hideCustom = false;
    
    do {
        showMenu(hideLab1, hideLab2, hideLab3, hideLab4, hideLab5, hideCustom);
        userChoice = readInteger("");
        
        switch (userChoice) {
            // =========================================================
            case 1:  // Generate Graph
            {
                std::cout << "\n--- Graph Generation ---\n";
                int numVertices = readInteger("Number of vertices: ");

                std::cout << "Graph type (1=Directed, 2=Undirected): ";
                int typeOption;
                while (!(std::cin >> typeOption) || typeOption < 1 || typeOption > 2) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid. Enter 1 or 2: ";
                }
                bool isDirected = (typeOption == 1);

                WeightSign wSign = selectWeightSign();

                AcyclicGraphBuilder builder;
                currentGraph = builder.generateAcyclicGraph(
                    numVertices, isDirected, wSign);
                currentFlowNetwork.reset();
                currentSpanningTree.reset();
                hasPruferCode = false;
                lastPruferCode = PruferCode{};
                lastFundamentalCuts.clear();
                lastMaxFlow = 0;
                lastFlowSource = -1;
                lastFlowSink = -1;
                hasMaxFlowResult = false;

                std::cout << "\n[OK] Graph created successfully!\n";
                std::cout << "    Vertices: " << numVertices << "\n";
                std::cout << "    Edges:    " << currentGraph->edges().size() << "\n";
                std::cout << "    Degree params:  n=" << BINOMIAL_N_DEGREE << ", p=" << BINOMIAL_P_DEGREE << "\n";
                std::cout << "    Weight params:  n=" << BINOMIAL_N_WEIGHT << ", p=" << BINOMIAL_P_WEIGHT << "\n";

                showDistributionInfo();
                showGraphStats(currentGraph);
                break;
            }
            
            // =========================================================
            case 2:  // Eccentricity Analysis (by edge count)
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }

                std::cout << "\n--- Eccentricity Computation (by edge count) ---\n";
                GraphAnalyzer analyzer(*currentGraph);
                auto result = analyzer.analyze();

                std::cout << "\nVertex Eccentricities (in edges):\n";
                // TO SHOW "unreachable" FOR END VERTICES:
                // Replace the loop with:
                // for (const auto& [vertex, ecc] : result.eccentricities) {
                //     if (ecc >= 0) {
                //         std::cout << "  v" << vertex << ": " << ecc << "\n";
                //     } else {
                //         std::cout << "  v" << vertex << ": unreachable\n";
                //     }
                // }
                for (const auto& [vertex, ecc] : result.eccentricities) {
                    std::cout << "  v" << vertex << ": " << ecc << "\n";
                }

                if (result.radius >= 0) {
                    std::cout << "\nRadius:     " << result.radius << " (edges)\n";
                    std::cout << "Diameter:   " << result.diameter << " (edges)\n";
                } else {
                    // TO REVERT: change this to "disconnected graph" when reverting end vertices to -1
                    std::cout << "\nRadius:     N/A (no vertices)\n";
                    std::cout << "Diameter:   N/A (no vertices)\n";
                }

                std::cout << "Center:     ";
                for (int v : result.centerVertices) std::cout << "v" << v << " ";
                std::cout << "\n";

                std::cout << "Diametrical vertices: ";
                for (int v : result.diametricalVertices) std::cout << "v" << v << " ";
                std::cout << "\n";

                // Print all diametrical paths grouped by vertex pair
                if (!result.diametricalPathsByPair.empty()) {
                    size_t totalPaths = 0;
                    for (const auto& [pair, paths] : result.diametricalPathsByPair) {
                        totalPaths += paths.size();
                    }
                    std::cout << "\nDiametrical paths (" << totalPaths << " total):\n";
                    
                    for (const auto& [pair, paths] : result.diametricalPathsByPair) {
                        std::cout << "\n  Between v" << pair.first << " and v" << pair.second << " (" << paths.size() << " paths):\n";
                        for (const auto& path : paths) {
                            std::cout << "    ";
                            for (size_t j = 0; j < path.size(); ++j) {
                                std::cout << "v" << path[j];
                                if (j < path.size() - 1) std::cout << " -> ";
                            }
                            std::cout << "\n";
                        }
                    }
                }
                break;
            }
            
            // =========================================================
            case 3:  // Shimbell's Method
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }
                
                int k = readNonNegativeIntegerStrict(
                    "\nPath length k (number of edges): ",
                    "[ERROR] k must be a non-negative integer number of edges: "
                );

                try {
                    KPathCalculator calculator(*currentGraph);
                    auto result = calculator.compute(k);
                    
                    auto vertexIds = currentGraph->vertexIds();
                    displayMatrix("Minimum Weight Paths", result.minWeights, vertexIds);
                    displayMatrix("Maximum Weight Paths", result.maxWeights, vertexIds);
                } catch (const std::exception& ex) {
                    std::cout << "[ERROR] " << ex.what() << "\n";
                }
                break;
            }
            
            // =========================================================
            case 4:  // Path Counting
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }

                int startVertex = readInteger("\nStart vertex: ");
                int endVertex = readInteger("End vertex: ");

                if (!currentGraph->hasVertex(startVertex) ||
                    !currentGraph->hasVertex(endVertex)) {
                    std::cout << "[FAIL] Invalid vertices\n";
                    break;
                }

                SimplePathFinder finder(*currentGraph);
                auto allPaths = finder.findAllPaths(startVertex, endVertex);

                if (allPaths.empty()) {
                    std::cout << "[FAIL] No paths found\n";
                } else {
                    std::cout << "[OK] Found " << allPaths.size() << " path(s)\n";

                    // Display ALL paths
                    std::cout << "Paths:\n";
                    for (size_t i = 0; i < allPaths.size(); ++i) {
                        std::cout << "  ";
                        for (size_t j = 0; j < allPaths[i].size(); ++j) {
                            std::cout << "v" << allPaths[i][j];
                            if (j < allPaths[i].size() - 1) std::cout << " -> ";
                        }
                        std::cout << "\n";
                    }
                }
                break;
            }
            
            // =========================================================
            case 5:  // Graph Details
            {
                showGraphDetails(currentGraph);
                break;
            }
            
            // =========================================================
            case 6:  // Distribution Comparison
            {
                showDistributionInfo();
                showGraphStats(currentGraph);
                break;
            }

            // =========================================================
            case 7:  // Show Adjacency Matrix
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }

                std::cout << "\n=== ADJACENCY MATRIX (direct edges) ===\n";
                std::cout << "Shows direct edges between vertices (1 edge)\n";
                std::cout << "1 = edge exists, 0 = no edge\n";
                auto matrix = currentGraph->getAdjacencyMatrix();
                printAdjacencyMatrix(std::cout, matrix, currentGraph->vertexIds());
                break;
            }

            // =========================================================
            case 8:  // Show Weight Matrix (Shimbell k=1)
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }

                std::cout << "\n=== WEIGHT MATRIX (Shimbell, k=1) ===\n";
                std::cout << "Minimum weight paths with EXACTLY 1 edge\n";
                std::cout << "i = no path with exactly 1 edge\n";
                try {
                    KPathCalculator calculator(*currentGraph);
                    auto result = calculator.compute(1);
                    auto vertexIds = currentGraph->vertexIds();
                    printWeightTable(std::cout, result.minWeights, vertexIds);
                } catch (const std::exception& ex) {
                    std::cout << "[ERROR] " << ex.what() << "\n";
                }
                break;
            }

            // =========================================================
            case 9:  // BFS Traversal (Lab 2)
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }

                std::cout << "\n--- BFS Traversal ---\n";
                int startVertex = readInteger("Start vertex: ");

                if (!currentGraph->hasVertex(startVertex)) {
                    std::cout << "[!] Invalid vertex\n";
                    break;
                }

                BreadthFirstSearch bfs(*currentGraph);
                BFSResult result = bfs.traverseWithLevels(startVertex);

                std::cout << "\n[OK] BFS completed in " << result.iterations << " iterations\n";
                std::cout << "Maximum depth (levels): " << result.maxLevel << "\n";

                // Display levels
                std::cout << "\n--- BFS Levels ---\n";
                for (int level = 0; level <= result.maxLevel; ++level) {
                    std::cout << "Level " << level << ": [";
                    const auto& vertices = result.levels.at(level);
                    for (size_t i = 0; i < vertices.size(); ++i) {
                        std::cout << vertices[i];
                        if (i < vertices.size() - 1) std::cout << ", ";
                    }
                    std::cout << "]\n";
                }

                // Check if graph is fully connected
                /*
                if (result.traversalOrder.size() < static_cast<size_t>(currentGraph->size())) {
                    std::cout << "\n[!] Warning: Graph may be disconnected\n";
                    std::cout << "    Visited " << result.traversalOrder.size() 
                              << " of " << currentGraph->size() << " vertices\n";
                }
                              */
                break;
            }

            // =========================================================
            case 10:  // Dijkstra's Algorithm (Lab 2)
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }

                std::cout << "\n--- Dijkstra's Shortest Path ---\n";
                int startVertex = readInteger("Start vertex: ");
                int endVertex = readInteger("End vertex: ");

                if (!currentGraph->hasVertex(startVertex) ||
                    !currentGraph->hasVertex(endVertex)) {
                    std::cout << "[!] Invalid vertices\n";
                    break;
                }

                DijkstraAlgorithm dijkstra(*currentGraph);
                DijkstraResult result = dijkstra.findShortestPaths(startVertex, endVertex);

                std::cout << "\n[OK] Dijkstra completed in " << result.iterations << " iterations\n";

                if (result.shortestPath.empty() || result.targetDistance == std::numeric_limits<double>::infinity()) {
                    std::cout << "[!] No path found from v" << startVertex << " to v" << endVertex << "\n";
                } else {
                    std::cout << "Shortest distance: " << result.targetDistance << "\n";
                    std::cout << "Path: ";
                    for (size_t i = 0; i < result.shortestPath.size(); ++i) {
                        std::cout << "v" << result.shortestPath[i];
                        if (i < result.shortestPath.size() - 1) std::cout << " -> ";
                    }
                    std::cout << "\n";
                }

                // Show distance vector
                std::cout << "\n--- Distance Vector from v" << startVertex << " ---\n";
                for (const auto& [vertex, dist] : result.distances) {
                    if (dist == std::numeric_limits<double>::infinity()) {
                        std::cout << "  v" << vertex << ": unreachable\n";
                    } else {
                        std::cout << "  v" << vertex << ": " << dist << "\n";
                    }
                }
                break;
            }

            // =========================================================
            case 11:  // Compare BFS vs Dijkstra (Lab 2)
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }

                std::cout << "\n--- Algorithm Speed Comparison ---\n";
                int startVertex = readInteger("Start vertex: ");

                if (!currentGraph->hasVertex(startVertex)) {
                    std::cout << "[!] Invalid vertex\n";
                    break;
                }

                AlgorithmComparator comparator(*currentGraph);
                AlgorithmComparison comparison = comparator.compare(startVertex);

                std::cout << "\n=== Comparison Results ===\n";
                std::cout << "BFS iterations:       " << comparison.bfsIterations << "\n";
                std::cout << "Dijkstra iterations:  " << comparison.dijkstraIterations << "\n";
                
                if (comparison.bfsIterations > 0) {
                    std::cout << "BFS is " << std::fixed << std::setprecision(2) 
                              << comparison.speedupFactor << "x times faster than Dijkstra.\n";
                }

                // Reset output formatting to default
                std::cout << std::resetiosflags(std::ios_base::floatfield) << std::setprecision(6);

                /*
                std::cout << "\n--- Analysis ---\n";
                if (comparison.bfsIterations < comparison.dijkstraIterations) {
                    std::cout << "BFS is faster for simple traversal (no weights).\n";
                    std::cout << "Dijkstra requires more iterations due to priority queue operations.\n";
                } else if (comparison.bfsIterations > comparison.dijkstraIterations) {
                    std::cout << "Unusual: Dijkstra appears faster. This may occur on sparse graphs.\n";
                } else {
                    std::cout << "Both algorithms required the same number of iterations.\n";
                }
                */
                break;
            }

            // =========================================================
            case 12:  // Build Flow Network
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }
                if (!currentGraph->isDirected()) {
                    std::cout << "[!] Lab 3 requires a directed graph. Generate a directed graph first.\n";
                    break;
                }

                FlowNetworkBuilder builder;
                currentFlowNetwork = builder.buildFromGraph(*currentGraph);
                lastMaxFlow = 0;
                lastFlowSource = -1;
                lastFlowSink = -1;
                hasMaxFlowResult = false;

                std::cout << "\n[OK] Flow network built from the current graph.\n";
                std::cout << "    Capacity(u, v) is generated randomly via the program generator\n";
                std::cout << "    Cost(u, v) is generated randomly via the program generator\n";
                std::cout << "    Vertices: " << currentFlowNetwork->size() << "\n";
                std::cout << "    Edges:    " << currentFlowNetwork->edges().size() << "\n";
                break;
            }

            // =========================================================
            case 13:  // Show Capacity Matrix
            {
                if (!currentFlowNetwork) {
                    std::cout << "[!] Build the flow network first\n";
                    break;
                }

                std::cout << "\n=== CAPACITY MATRIX ===\n";
                std::cout << "i = no directed edge\n";
                printFlowMatrix(std::cout,
                                currentFlowNetwork->getCapacityMatrix(),
                                currentFlowNetwork->vertexIds());
                break;
            }

            // =========================================================
            case 14:  // Show Cost Matrix
            {
                if (!currentFlowNetwork) {
                    std::cout << "[!] Build the flow network first\n";
                    break;
                }

                std::cout << "\n=== COST MATRIX ===\n";
                std::cout << "i = no directed edge\n";
                printFlowMatrix(std::cout,
                                currentFlowNetwork->getCostMatrix(),
                                currentFlowNetwork->vertexIds());
                break;
            }

            // =========================================================
            case 15:  // Maximum Flow
            {
                if (!currentFlowNetwork) {
                    std::cout << "[!] Build the flow network first\n";
                    break;
                }

                std::cout << "\n--- Maximum Flow (Ford-Fulkerson) ---\n";
                int source = readInteger("Source vertex: ");
                int sink = readInteger("Sink vertex: ");

                if (!currentFlowNetwork->hasVertex(source) ||
                    !currentFlowNetwork->hasVertex(sink) ||
                    source == sink) {
                    std::cout << "[!] Invalid source/sink pair\n";
                    break;
                }

                MaxFlowSolver solver(*currentFlowNetwork);
                MaxFlowResult result = solver.compute(source, sink);

                lastMaxFlow = result.maxFlow;
                lastFlowSource = source;
                lastFlowSink = sink;
                hasMaxFlowResult = true;

                std::cout << "\n[OK] Maximum flow = " << result.maxFlow << "\n";
                if (result.steps.empty()) {
                    if (sinkHasOnlyZeroCapacityIncomingEdges(*currentFlowNetwork, sink)) {
                        std::cout << "[!] No paths from source to sink: all edges leading to the sink have capacity = 0\n";
                    } else {
                        std::cout << "[!] No augmenting path found\n";
                    }
                } else {
                    std::cout << "\nAugmenting steps:\n";
                    for (const MaxFlowStep& step : result.steps) {
                        std::cout << "  Step " << step.iteration << ": ";
                        printVertexPath(step.path);
                        std::cout << " | added flow = " << step.pathFlow
                                  << " | total = " << step.totalFlow << "\n";
                    }
                }

                std::cout << "\nFlow matrix after max flow:\n";
                printFlowMatrix(std::cout,
                                currentFlowNetwork->getFlowMatrix(),
                                currentFlowNetwork->vertexIds());
                break;
            }

            // =========================================================
            case 16:  // Minimum-Cost Flow
            {
                if (!currentFlowNetwork) {
                    std::cout << "[!] Build the flow network first\n";
                    break;
                }
                if (!hasMaxFlowResult) {
                    std::cout << "[!] Compute maximum flow first\n";
                    break;
                }

                const int targetFlow = (2 * lastMaxFlow) / 3;

                std::cout << "\n--- Minimum-Cost Flow ---\n";
                std::cout << "Using the same source/sink as the maximum-flow run: "
                          << "v" << lastFlowSource << " -> v" << lastFlowSink << "\n";
                std::cout << "Target flow = floor(2/3 * " << lastMaxFlow
                          << ") = " << targetFlow << "\n";

                MinCostFlowSolver solver(*currentFlowNetwork);
                MinCostFlowResult result = solver.compute(lastFlowSource, lastFlowSink, targetFlow);

                std::cout << "\n=== Result ===\n";
                std::cout << "Target flow:     " << result.targetFlow << "\n";
                std::cout << "Achieved flow:   " << result.achievedFlow << "\n";
                std::cout << "Total cost:      " << result.totalCost << "\n";
                std::cout << "Success:         " << (result.success ? "yes" : "no") << "\n";

                if (result.achievedFlow == 0 &&
                    sinkHasOnlyZeroCapacityIncomingEdges(*currentFlowNetwork, lastFlowSink)) {
                    std::cout << "[!] No paths from source to sink: all edges leading to the sink have capacity = 0\n";
                }

                if (!result.steps.empty()) {
                    std::cout << "\nAugmenting steps:\n";
                    for (const MinCostFlowStep& step : result.steps) {
                        std::cout << "  Step " << step.iteration << ": ";
                        printVertexPath(step.path);
                        std::cout << "\n    added flow:       " << step.pathFlow
                                  << "\n    path unit cost:   " << step.pathUnitCost
                                  << "\n    cumulative flow:  " << step.cumulativeFlow
                                  << "\n    cumulative cost:  " << step.cumulativeCost
                                  << "\n";
                    }
                }

                std::cout << "\nFlow matrix after minimum-cost flow:\n";
                printFlowMatrix(std::cout,
                                currentFlowNetwork->getFlowMatrix(),
                                currentFlowNetwork->vertexIds());
                break;
            }

            // =========================================================
            case 17:  // Flow Network Details
            {
                showFlowNetworkDetails(currentFlowNetwork);
                break;
            }

            // =========================================================
            case 18:  // Count spanning trees (Kirchhoff)
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }
                if (currentGraph->isDirected()) {
                    std::cout << "[!] Lab 4 requires an undirected graph. "
                                 "Generate an undirected graph first.\n";
                    break;
                }

                KirchhoffCounter counter(*currentGraph);
                KirchhoffResult result = counter.compute();

                std::cout << "\n=== KIRCHHOFF MATRIX (B = D - A) ===\n";
                std::cout << "Diagonal: degree of vertex.\n";
                std::cout << "Off-diagonal: -1 if edge exists, 0 otherwise.\n\n";

                const auto vertexIds = currentGraph->vertexIds();
                const int n = currentGraph->size();

                std::cout << "      ";
                for (int id : vertexIds) std::cout << std::setw(6) << id;
                std::cout << "\n      ";
                for (int i = 0; i < n; ++i) std::cout << "------";
                std::cout << "\n";
                for (int i = 0; i < n; ++i) {
                    std::cout << std::setw(5) << vertexIds[i] << " |";
                    for (int j = 0; j < n; ++j) {
                        std::cout << std::setw(6) << result.kirchhoffMatrix[i][j];
                    }
                    std::cout << "\n";
                }

                std::cout << "\n[OK] Number of spanning trees = "
                          << result.spanningTreeCount << "\n";
                if (result.spanningTreeCount == 0 && n > 1) {
                    std::cout << "[!] Zero spanning trees -- the graph is disconnected.\n";
                }
                break;
            }

            // =========================================================
            case 19:  // Build MST (Kruskal)
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }
                if (currentGraph->isDirected()) {
                    std::cout << "[!] Lab 4 requires an undirected graph. "
                                 "Generate an undirected graph first.\n";
                    break;
                }

                KruskalMST kruskal(*currentGraph);
                KruskalResult result = kruskal.buildMST();

                std::cout << "\n=== KRUSKAL'S ALGORITHM ===\n";
                std::cout << "Edges added (in order, sorted by ascending weight):\n";
                for (size_t i = 0; i < result.chosenEdges.size(); ++i) {
                    const auto& e = result.chosenEdges[i];
                    std::cout << "  " << (i + 1) << ". v" << e.from
                              << " --- v" << e.to
                              << "  (weight = " << e.weight << ")\n";
                }

                std::cout << "\nTotal weight of MST: " << result.totalWeight << "\n";
                std::cout << "Edges in MST:        " << result.chosenEdges.size()
                          << " of " << (currentGraph->size() - 1) << " expected\n";

                if (!result.wasConnected) {
                    std::cout << "[!] Graph was disconnected -- result is a "
                                 "spanning forest, not a tree.\n";
                }

                currentSpanningTree = std::move(result.spanningTree);
                hasPruferCode = false;  // tree has changed, invalidate old code
                lastPruferCode = PruferCode{};
                lastFundamentalCuts.clear();  // cuts depend on the tree
                std::cout << "\n[OK] Spanning tree saved as the current MST.\n";
                break;
            }

            // =========================================================
            case 20:  // Show MST details
            {
                if (!currentSpanningTree) {
                    std::cout << "[!] Build the spanning tree first (option 19)\n";
                    break;
                }

                std::cout << "\n=== SPANNING TREE ===\n";
                std::cout << "Type:     "
                          << (currentSpanningTree->isDirected() ? "Directed" : "Undirected")
                          << "\n";
                std::cout << "Vertices: " << currentSpanningTree->size() << "\n";
                std::cout << "Edges:    " << currentSpanningTree->edges().size() << "\n";

                double totalWeight = 0.0;
                std::cout << "\nEdges:\n";
                for (const WeightedEdge& edge : currentSpanningTree->edges()) {
                    std::cout << "  v" << edge.from << " --- v" << edge.to
                              << "  (weight = " << edge.weight << ")\n";
                    totalWeight += edge.weight;
                }
                std::cout << "\nTotal weight: " << totalWeight << "\n";
                break;
            }

            // =========================================================
            case 21:  // Encode tree to Prufer code
            {
                if (!currentSpanningTree) {
                    std::cout << "[!] Build the spanning tree first (option 19)\n";
                    break;
                }

                lastPruferCode = PruferEncoder::encode(*currentSpanningTree);
                hasPruferCode = true;

                const int p = currentSpanningTree->size();
                std::cout << "\n=== PRUFER CODE ===\n";
                std::cout << "Tree has " << p << " vertices, code length = p - 1 = "
                          << (p - 1) << "\n\n";

                std::cout << "Code:    [";
                for (size_t i = 0; i < lastPruferCode.code.size(); ++i) {
                    std::cout << lastPruferCode.code[i];
                    if (i + 1 < lastPruferCode.code.size()) std::cout << ", ";
                }
                std::cout << "]\n";

                std::cout << "Weights: [";
                for (size_t i = 0; i < lastPruferCode.weights.size(); ++i) {
                    std::cout << lastPruferCode.weights[i];
                    if (i + 1 < lastPruferCode.weights.size()) std::cout << ", ";
                }
                std::cout << "]\n";

                std::cout << "\n(Step i: removed smallest leaf -> recorded its "
                             "neighbor in code[i] and the edge weight in weights[i])\n";
                break;
            }

            // =========================================================
            case 22:  // Decode last Prufer code
            {
                if (!hasPruferCode) {
                    std::cout << "[!] Encode a tree first (option 21)\n";
                    break;
                }
                if (!currentSpanningTree) {
                    std::cout << "[!] Need the original tree to know its vertex set\n";
                    break;
                }

                auto decoded = PruferEncoder::decode(
                    lastPruferCode, currentSpanningTree->vertexIds());

                std::cout << "\n=== DECODED TREE ===\n";
                std::cout << "Vertices: " << decoded->size() << "\n";
                std::cout << "Edges:    " << decoded->edges().size() << "\n\n";

                std::cout << "Edges:\n";
                for (const WeightedEdge& edge : decoded->edges()) {
                    std::cout << "  v" << edge.from << " --- v" << edge.to
                              << "  (weight = " << edge.weight << ")\n";
                }

                // Round-trip check: decoded edges should match the original tree.
                auto originalEdges = currentSpanningTree->edges();
                auto decodedEdges = decoded->edges();

                auto edgeKey = [](const WeightedEdge& e) {
                    int a = e.from, b = e.to;
                    if (a > b) std::swap(a, b);
                    return std::make_tuple(a, b, e.weight);
                };

                std::vector<std::tuple<int, int, double>> originalKeys, decodedKeys;
                for (const auto& e : originalEdges) originalKeys.push_back(edgeKey(e));
                for (const auto& e : decodedEdges)  decodedKeys.push_back(edgeKey(e));
                std::sort(originalKeys.begin(), originalKeys.end());
                std::sort(decodedKeys.begin(),  decodedKeys.end());

                if (originalKeys == decodedKeys) {
                    std::cout << "\n[OK] Round-trip successful: decoded tree "
                                 "matches the original (edges + weights).\n";
                } else {
                    std::cout << "\n[!] Round-trip MISMATCH between original and decoded tree.\n";
                }
                break;
            }

            // =========================================================
            case 23:  // Maximal matching
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }
                if (currentGraph->isDirected()) {
                    std::cout << "[!] Lab 4 requires an undirected graph. "
                                 "Generate an undirected graph first.\n";
                    break;
                }

                std::cout << "\n--- Maximal Matching (independent edge set) ---\n";
                std::cout << "Run on:\n";
                std::cout << "  1. Original graph\n";
                std::cout << "  2. Spanning tree (option 19 must be done first)\n";
                std::cout << "> ";

                int targetOption;
                while (!(std::cin >> targetOption) ||
                       (targetOption != 1 && targetOption != 2)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid. Enter 1 or 2: ";
                }

                const AdjacencyGraph* target = nullptr;
                std::string targetName;
                if (targetOption == 1) {
                    target = currentGraph.get();
                    targetName = "original graph";
                } else {
                    if (!currentSpanningTree) {
                        std::cout << "[!] Build the spanning tree first (option 19)\n";
                        break;
                    }
                    target = currentSpanningTree.get();
                    targetName = "spanning tree";
                }

                GreedyMaximalMatching matcher(*target);
                MatchingResult result = matcher.compute();

                std::cout << "\n=== MAXIMAL MATCHING on " << targetName << " ===\n";
                std::cout << "Matching size: " << result.matchingSize << "\n";

                std::cout << "\nMatching edges:\n";
                if (result.matchingEdges.empty()) {
                    std::cout << "  (none)\n";
                } else {
                    for (const WeightedEdge& edge : result.matchingEdges) {
                        std::cout << "  v" << edge.from << " --- v" << edge.to
                                  << "  (weight = " << edge.weight << ")\n";
                    }
                }

                if (result.unmatchedVertices.empty()) {
                    std::cout << "\n[OK] Matching is PERFECT -- every vertex is covered.\n";
                } else {
                    std::cout << "\nUnmatched vertices: ";
                    for (int v : result.unmatchedVertices) std::cout << "v" << v << " ";
                    std::cout << "\n";
                }
                break;
            }

            // =========================================================
            case 24:  // Build Eulerian cycle
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }
                if (currentGraph->isDirected()) {
                    std::cout << "[!] Lab 5 (Eulerian cycle) requires an undirected graph.\n";
                    break;
                }

                EulerianCycleBuilder builder(*currentGraph);
                // First attempt: try to eulerize without creating a multigraph.
                EulerianCycleResult er = builder.compute(EulerizationMode::NonMultigraphOnly);

                std::cout << "\n=== EULERIAN CYCLE ===\n";

                // Handle the "needs multigraph" case: ask the user whether to
                // proceed with parallel edges or to abort.
                if (!er.success && er.requiresMultigraph) {
                    std::cout << "[!] Cannot make this graph Eulerian without\n"
                                 "    duplicating an existing edge -- the only\n"
                                 "    possible eulerization turns it into a\n"
                                 "    multigraph.\n";
                    std::cout << "Modify the graph into a multigraph anyway to\n"
                                 "obtain an Eulerian cycle? [y/n]: ";
                    std::string answer;
                    //std::getline(std::cin, answer);
                    // Accept "y" / "Y" / "yes" / "Yes" / etc.
                    std::cin >> answer;
                    bool accept = answer[0] == 'y';
                    if (!accept) {
                        std::cout << "[i] Aborted -- no Eulerian cycle was built.\n";
                        break;
                    }

                    // Re-run with multigraph allowed.
                    er = builder.compute(EulerizationMode::AllowMultigraph);
                    if (!er.success) {
                        std::cout << "[!] Unexpected failure while building\n"
                                     "    the Eulerian cycle.\n";
                        break;
                    }
                    std::cout << "[i] Proceeding with multigraph eulerization.\n";
                }

                if (!er.success) {
                    std::cout << "[!] Cannot build Eulerian cycle (graph has no edges).\n";
                    break;
                }

                // Report whether modifications were needed.
                if (er.wasAlreadyEulerian) {
                    std::cout << "[OK] Graph is already Eulerian "
                                 "(connected + all degrees even).\n";
                } else {
                    std::cout << "[!] Graph was NOT Eulerian. Modifications applied:\n";
                    for (size_t i = 0; i < er.additions.size(); ++i) {
                        const auto& a = er.additions[i];
                        std::cout << "  " << (i + 1) << ". added edge v" << a.from
                                  << " --- v" << a.to
                                  << "  [" << a.reason << "]\n";
                    }
                    std::cout << "Total edges added: " << er.additions.size() << "\n";
                }

                // Print the cycle as a vertex sequence v0 -> v1 -> ... -> v0.
                std::cout << "\nEulerian cycle (" << (er.cycle.size() - 1)
                          << " edges, " << er.cycle.size() << " vertex stops):\n";
                std::cout << "  ";
                for (size_t i = 0; i < er.cycle.size(); ++i) {
                    std::cout << "v" << er.cycle[i];
                    if (i + 1 < er.cycle.size()) std::cout << " -> ";
                    // Wrap long lines for readability.
                    if ((i + 1) % 12 == 0 && i + 1 < er.cycle.size()) {
                        std::cout << "\n  ";
                    }
                }
                std::cout << "\n";
                break;
            }

            // =========================================================
            case 25:  // Fundamental cut-set system
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }
                if (currentGraph->isDirected()) {
                    std::cout << "[!] Lab 5 requires an undirected graph.\n";
                    break;
                }
                if (!currentSpanningTree) {
                    std::cout << "[!] Build the spanning tree first (option 19)\n";
                    break;
                }

                FundamentalCutSystem system(*currentGraph, *currentSpanningTree);
                lastFundamentalCuts = system.compute();

                std::cout << "\n=== FUNDAMENTAL CUT-SET SYSTEM ===\n";
                std::cout << "One fundamental cut per tree edge "
                          << "(" << lastFundamentalCuts.size() << " cuts).\n";
                std::cout << "Each cut Q(e) = all graph edges with endpoints on\n";
                std::cout << "opposite sides after removing tree edge e.\n";

                for (size_t i = 0; i < lastFundamentalCuts.size(); ++i) {
                    const FundamentalCut& cut = lastFundamentalCuts[i];
                    std::cout << "\n  Q" << (i + 1)
                              << " (tree edge v" << cut.treeEdgeFrom
                              << " --- v" << cut.treeEdgeTo << "):\n";

                    std::cout << "    Side A: { ";
                    for (int v : cut.sideA) std::cout << "v" << v << " ";
                    std::cout << "}\n";

                    std::cout << "    Side B: { ";
                    for (int v : cut.sideB) std::cout << "v" << v << " ";
                    std::cout << "}\n";

                    std::cout << "    Edges (" << cut.edges.size() << "): ";
                    for (size_t j = 0; j < cut.edges.size(); ++j) {
                        std::cout << "(v" << cut.edges[j].first
                                  << ",v" << cut.edges[j].second << ")";
                        if (j + 1 < cut.edges.size()) std::cout << ", ";
                    }
                    std::cout << "\n";
                }
                break;
            }

            // =========================================================
            case 26:  // Combine cuts via symmetric difference
            {
                if (lastFundamentalCuts.empty()) {
                    std::cout << "[!] Build the fundamental cut-set system first (option 25)\n";
                    break;
                }

                std::cout << "\n--- Symmetric Difference of Cuts ---\n";
                std::cout << "Available fundamental cuts: 1.." << lastFundamentalCuts.size() << "\n";
                std::cout << "Enter cut numbers separated by spaces, "
                             "end with 0 (need at least 2):\n";
                std::cout << "> ";

                std::vector<int> picks;
                while (true) {
                    int x;
                    if (!(std::cin >> x)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Invalid token, try again: ";
                        continue;
                    }
                    if (x == 0) break;
                    if (x < 1 || x > static_cast<int>(lastFundamentalCuts.size())) {
                        std::cout << "Out of range, ignored. Continue: ";
                        continue;
                    }
                    picks.push_back(x);
                }

                if (picks.size() < 2) {
                    std::cout << "[!] Need at least 2 cuts to take a symmetric difference.\n";
                    break;
                }

                // XOR the picks together left-to-right.
                // Since (A xor B) xor C = A xor (B xor C), order does not change
                // the final set, but we accumulate left to right anyway.
                std::vector<std::pair<int,int>> acc = lastFundamentalCuts[picks[0] - 1].edges;
                for (size_t i = 1; i < picks.size(); ++i) {
                    acc = FundamentalCutSystem::symmetricDifference(
                        acc, lastFundamentalCuts[picks[i] - 1].edges);
                }

                std::cout << "\nResult of Q" << picks[0];
                for (size_t i = 1; i < picks.size(); ++i) {
                    std::cout << " XOR Q" << picks[i];
                }
                std::cout << ":\n";
                std::cout << "  Edges (" << acc.size() << "): ";
                if (acc.empty()) {
                    std::cout << "(empty cut)";
                } else {
                    for (size_t j = 0; j < acc.size(); ++j) {
                        std::cout << "(v" << acc[j].first
                                  << ",v" << acc[j].second << ")";
                        if (j + 1 < acc.size()) std::cout << ", ";
                    }
                }
                std::cout << "\n";
                break;
            }

            // =========================================================
            case 1001:  // Toggle hiding legacy sections
            {
                hideLab1 = !hideLab1;
                std::cout << (hideLab1
                    ? "[OK] Lab 1 is now hidden in the menu.\n"
                    : "[OK] Lab 1 is now shown in the menu.\n");
                break;
            }

            case 1002:  // Toggle hiding legacy sections
            {
                hideLab2 = !hideLab2;
                std::cout << (hideLab2
                    ? "[OK] Lab 2 is now hidden in the menu.\n"
                    : "[OK] Lab 2 is now shown in the menu.\n");
                break;
            }

            case 1003:  // Toggle hiding legacy sections
            {
                hideLab3 = !hideLab3;
                std::cout << (hideLab3
                    ? "[OK] Lab 3 is now hidden in the menu.\n"
                    : "[OK] Lab 3 is now shown in the menu.\n");
                break;
            }

            case 1004:  // Toggle hiding legacy sections
            {
                hideLab4 = !hideLab4;
                std::cout << (hideLab4
                    ? "[OK] Lab 4 is now hidden in the menu.\n"
                    : "[OK] Lab 4 is now shown in the menu.\n");
                break;
            }

            case 1005:  // Toggle hiding legacy sections
            {
                hideLab5 = !hideLab5;
                std::cout << (hideLab5
                    ? "[OK] Lab 5 is now hidden in the menu.\n"
                    : "[OK] Lab 5 is now shown in the menu.\n");
                break;
            }

            case 1006:  // Toggle hiding legacy sections
            {
                hideCustom = !hideCustom;
                std::cout << (hideCustom
                    ? "[OK] Custom menu is now hidden in the menu.\n"
                    : "[OK] Custom menu is now shown in the menu.\n");
                break;
            }

            // =========================================================
            case 102:  // Export Flow Network Mermaid
            {
                if (!currentFlowNetwork) {
                    std::cout << "[!] Build the flow network first\n";
                    break;
                }

                try {
                    const auto outputPath = MermaidExporter::exportFlowNetwork(*currentFlowNetwork);
                    std::cout << "\n[OK] Flow-network Mermaid code written to:\n"
                              << "    " << outputPath.string() << "\n";
                } catch (const std::exception& ex) {
                    std::cout << "[ERROR] " << ex.what() << "\n";
                }
                break;
            }

            // =========================================================
            case 101:  // Export Graph Mermaid
            {
                if (!currentGraph) {
                    std::cout << "[!] Generate a graph first\n";
                    break;
                }

                try {
                    const auto outputPath = MermaidExporter::exportGraph(*currentGraph);
                    std::cout << "\n[OK] Mermaid code written to:\n"
                              << "    " << outputPath.string() << "\n";
                } catch (const std::exception& ex) {
                    std::cout << "[ERROR] " << ex.what() << "\n";
                }
                break;
            }

            // =========================================================
            case 103:  // Export Spanning Tree Mermaid
            {
                if (!currentSpanningTree) {
                    std::cout << "[!] Build the spanning tree first (option 19)\n";
                    break;
                }

                try {
                    const auto outputPath = MermaidExporter::exportGraph(
                        *currentSpanningTree, "generated_spanning_tree.mmd");
                    std::cout << "\n[OK] Spanning-tree Mermaid code written to:\n"
                              << "    " << outputPath.string() << "\n";
                } catch (const std::exception& ex) {
                    std::cout << "[ERROR] " << ex.what() << "\n";
                }
                break;
            }

            // =========================================================
            case 0:  // Exit
            {
                std::cout << "Exiting program...\n";
                break;
            }
            
            // =========================================================
            default:  // Invalid Option
            {
                std::cout << "[!] Invalid choice. Please try again.\n";
            }
        }
        
    } while (userChoice != 0);
    
    return 0;
}
