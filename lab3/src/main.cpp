#include <iostream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <string>
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

// ============================================================================
// Helper Functions
// ============================================================================

void showMenu() {
    std::cout << "\n========== MAIN MENU ==========\n";
    std::cout << "  1. Generate random graph\n";
    std::cout << "  2. Calculate eccentricities & center\n";
    std::cout << "  3. Shimbell's method (k-edge paths)\n";
    std::cout << "  4. Find all paths between vertices\n";
    std::cout << "  5. Show graph details\n";
    std::cout << "  6. Compare distribution statistics\n";
    std::cout << "  7. Show adjacency matrix\n";
    std::cout << "  8. Show weight matrix (Shimbell k=1)\n";
    std::cout << "\n  --- Lab 2 ---\n";
    std::cout << "  9. BFS traversal\n";
    std::cout << "  10. Dijkstra's shortest path\n";
    std::cout << "  11. Compare BFS vs Dijkstra (speed)\n";
    std::cout << "\n  --- Lab 3 ---\n";
    std::cout << "  12. Build flow network from current directed graph\n";
    std::cout << "  13. Show capacity matrix\n";
    std::cout << "  14. Show cost matrix\n";
    std::cout << "  15. Find maximum flow\n";
    std::cout << "  16. Find minimum-cost flow for [2/3 * max]\n";
    std::cout << "  17. Show flow network details\n";
    std::cout << "  02. Export current flow network to Mermaid\n";
    std::cout << "  99. Export current graph to Mermaid\n";
    std::cout << "  0. Quit\n";
    std::cout << "===============================\n";
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

std::string readMenuCommand() {
    std::string command;
    while (!(std::cin >> command)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. > ";
    }
    return command;
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

// ============================================================================
// Main Program
// ============================================================================

int main() {
    std::unique_ptr<AdjacencyGraph> currentGraph;
    std::unique_ptr<FlowNetwork> currentFlowNetwork;
    int userChoice;
    std::string userChoiceRaw;
    double lastMaxFlow = 0.0;
    int lastFlowSource = -1;
    int lastFlowSink = -1;
    bool hasMaxFlowResult = false;
    
    do {
        showMenu();
        userChoiceRaw = readMenuCommand();

        if (userChoiceRaw == "02") {
            userChoice = 102;
        } else {
            try {
                size_t processedChars = 0;
                userChoice = std::stoi(userChoiceRaw, &processedChars);
                if (processedChars != userChoiceRaw.size()) {
                    userChoice = -1;
                }
            } catch (const std::exception&) {
                userChoice = -1;
            }
        }
        
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
                lastMaxFlow = 0.0;
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
                
                int k = readInteger("\nPath length k (number of edges): ");
                
                //try {
                    KPathCalculator calculator(*currentGraph);
                    auto result = calculator.compute(k);
                    
                    auto vertexIds = currentGraph->vertexIds();
                    displayMatrix("Minimum Weight Paths", result.minWeights, vertexIds);
                    displayMatrix("Maximum Weight Paths", result.maxWeights, vertexIds);
                //} catch (const std::exception& ex) { // i don't need it for now. FTF
                //    std::cout << "[ERROR] " << ex.what() << "\n";
                //}
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
                lastMaxFlow = 0.0;
                lastFlowSource = -1;
                lastFlowSink = -1;
                hasMaxFlowResult = false;

                std::cout << "\n[OK] Flow network built from the current graph.\n";
                std::cout << "    Capacity(u, v) = max(1, |weight(u, v)|)\n";
                std::cout << "    Cost(u, v) = original edge weight\n";
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
                    std::cout << "[!] No augmenting path found\n";
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

                const double targetFlow = std::floor((2.0 / 3.0) * lastMaxFlow);

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
            case 102:  // Export Flow Network Mermaid via 02
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
            case 99:  // Export Mermaid
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
