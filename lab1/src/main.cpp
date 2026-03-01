#include <iostream>
#include <limits>
#include "include/Graph.h"
#include "include/Generator.h"
#include "include/ShimbellMethod.h"
#include "include/PathCounter.h"
#include "include/Eccentricity.h"

using namespace graph;

void printMenu() {
    std::cout << "\n========== MENU ==========\n";
    std::cout << "1. Generate new graph\n";
    std::cout << "2. Compute eccentricities and find center\n";
    std::cout << "3. Shimbell method (min/max paths)\n";
    std::cout << "4. Count paths between vertices\n";
    std::cout << "5. Display graph info\n";
    std::cout << "6. Show distribution comparison\n";
    std::cout << "0. Exit\n";
    std::cout << "==========================\n";
    std::cout << "Choice: ";
}

int readInt(const std::string& prompt) {
    std::cout << prompt;
    int val;
    while (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. " << prompt;
    }
    return val;
}

WeightType readWeightType() {
    std::cout << "\nSelect weight type:\n";
    std::cout << "1. Positive only\n";
    std::cout << "2. Negative only\n";
    std::cout << "3. Mixed (positive and negative)\n";
    std::cout << "Choice: ";
    
    int choice;
    while (!(std::cin >> choice) || choice < 1 || choice > 3) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Choice (1-3): ";
    }
    
    switch (choice) {
        case 1: return WeightType::Positive;
        case 2: return WeightType::Negative;
        case 3: return WeightType::Mixed;
    }
    return WeightType::Positive;
}

void printMatrix(const std::string& title, const DistanceMatrix& mat, const std::vector<int>& ids) {
    std::cout << "\n=== " << title << " ===\n";
    std::cout << "   ";
    for (int id : ids) std::cout << "v" << id << " ";
    std::cout << "\n";
    for (size_t i = 0; i < mat.size(); ++i) {
        std::cout << "v" << ids[i] << " ";
        for (size_t j = 0; j < mat[i].size(); ++j) {
            if (mat[i][j]) std::cout << mat[i][j].value() << " ";
            else std::cout << "inf ";
        }
        std::cout << "\n";
    }
}

void printDistributionComparison() {
    std::cout << "\n=== Binomial Distribution Characteristics ===\n";
    
    auto theoretical = Generator::getTheoreticalCharacteristics(BINOMIAL_N, BINOMIAL_P);
    
    std::cout << "Theoretical (B(" << BINOMIAL_N << ", " << BINOMIAL_P << ")):\n";
    std::cout << "  Mean:     " << theoretical.mean << "\n";
    std::cout << "  Variance: " << theoretical.variance << "\n";
    std::cout << "  Std Dev:  " << theoretical.stdDev << "\n";
    std::cout << "  Mode:     " << theoretical.mode << "\n";
    std::cout << "  Skewness: " << theoretical.skewness << "\n";
    std::cout << "  Kurtosis: " << theoretical.kurtosis << "\n";
}

void printGraphStatistics(const std::unique_ptr<Graph>& graph) {
    if (!graph) {
        std::cout << "[!] No graph generated yet.\n";
        return;
    }
    
    std::cout << "\n=== Generated Graph Statistics ===\n";
    
    auto stats = Generator::computeGraphStatistics(*graph);
    
    std::cout << "Vertices:     " << graph->size() << "\n";
    std::cout << "Edges:        " << graph->edges().size() << "\n";
    std::cout << "Mean degree:  " << stats.meanDegree << "\n";
    std::cout << "Variance:     " << stats.variance << "\n";
    std::cout << "Std Dev:      " << stats.stdDev << "\n";
    std::cout << "Min degree:   " << stats.minDegree << "\n";
    std::cout << "Max degree:   " << stats.maxDegree << "\n";
    
    // Compare with theoretical
    auto theoretical = Generator::getTheoreticalCharacteristics(BINOMIAL_N, BINOMIAL_P);
    std::cout << "\n--- Comparison with Theoretical ---\n";
    std::cout << "Mean error:   " << std::abs(stats.meanDegree - theoretical.mean) << "\n";
    std::cout << "Var error:    " << std::abs(stats.variance - theoretical.variance) << "\n";
}

void displayGraphInfo(const std::unique_ptr<Graph>& graph) {
    if (!graph) {
        std::cout << "[!] No graph generated yet.\n";
        return;
    }
    
    std::cout << "\n=== Graph Information ===\n";
    std::cout << "Type: " << (graph->isDirected() ? "Directed" : "Undirected") << "\n";
    std::cout << "Vertices: " << graph->size() << "\n";
    std::cout << "Edges: " << graph->edges().size() << "\n";
    std::cout << "Vertex IDs: ";
    for (int id : graph->vertexIds()) {
        std::cout << id << " ";
    }
    std::cout << "\n";
}

int main() {
    std::unique_ptr<Graph> graph;
    int choice;
    
    do {
        printMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                // Generate new graph
                std::cout << "\n--- Graph Generation ---\n";
                int vertices = readInt("Number of vertices: ");
                int edges = readInt("Number of edges: ");
                
                std::cout << "Graph type: (1) Directed, (2) Undirected: ";
                int graphType;
                while (!(std::cin >> graphType) || graphType < 1 || graphType > 2) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Choice (1-2): ";
                }
                bool directed = (graphType == 1);
                
                WeightType weightType = readWeightType();
                
                Generator gen;
                graph = gen.generateAcyclicGraph(vertices, edges, directed, weightType);
                
                std::cout << "[OK] Graph generated (V=" << vertices << ", E=" << graph->edges().size() << ")\n";
                
                // Show distribution comparison
                printDistributionComparison();
                printGraphStatistics(graph);
                break;
            }
            
            case 2: {
                // Compute eccentricities
                if (!graph) {
                    std::cout << "[!] Generate a graph first.\n";
                    break;
                }
                
                std::cout << "\n--- Eccentricity Analysis ---\n";
                EccentricityCalculator calc(*graph);
                auto result = calc.compute();
                
                std::cout << "\nEccentricities:\n";
                for (auto const& [v, ecc] : result.eccentricities) {
                    std::cout << "  v" << v << ": " << ecc << "\n";
                }
                
                std::cout << "\nRadius: " << result.radius << "\n";
                std::cout << "Diameter: " << result.diameter << "\n";
                
                std::cout << "Center vertices: ";
                for (int v : result.center) {
                    std::cout << "v" << v << " ";
                }
                std::cout << "\n";
                
                std::cout << "Diametrical vertices: ";
                for (int v : result.diametrical) {
                    std::cout << "v" << v << " ";
                }
                std::cout << "\n";
                break;
            }
            
            case 3: {
                // Shimbell method
                if (!graph) {
                    std::cout << "[!] Generate a graph first.\n";
                    break;
                }
                
                int pathLen = readInt("\nPath length for Shimbell (k): ");
                
                try {
                    ShimbellMethod shimbell(*graph);
                    auto result = shimbell.compute(pathLen);
                    
                    auto ids = graph->vertexIds();
                    printMatrix("Minimum paths (Shimbell)", result.min_distances, ids);
                    printMatrix("Maximum paths (Shimbell)", result.max_distances, ids);
                } catch (const std::exception& e) {
                    std::cout << "[ERROR] " << e.what() << "\n";
                }
                break;
            }
            
            case 4: {
                // Count paths
                if (!graph) {
                    std::cout << "[!] Generate a graph first.\n";
                    break;
                }
                
                int from = readInt("\nStart vertex: ");
                int to = readInt("End vertex: ");
                
                if (!graph->hasVertex(from) || !graph->hasVertex(to)) {
                    std::cout << "[FAIL] Vertices do not exist\n";
                    break;
                }
                
                PathCounter counter(*graph);
                auto paths = counter.getAllPaths(from, to);
                
                if (paths.empty()) {
                    std::cout << "[FAIL] No paths found\n";
                } else {
                    std::cout << "[OK] Paths found: " << paths.size() << "\n";
                    std::cout << "Paths:\n";
                    for (size_t i = 0; i < paths.size() && i < 10; ++i) {
                        std::cout << "  ";
                        for (size_t j = 0; j < paths[i].size(); ++j) {
                            std::cout << "v" << paths[i][j];
                            if (j < paths[i].size() - 1) std::cout << " -> ";
                        }
                        std::cout << "\n";
                    }
                    if (paths.size() > 10) {
                        std::cout << "  ... and " << (paths.size() - 10) << " more\n";
                    }
                }
                break;
            }
            
            case 5: {
                displayGraphInfo(graph);
                break;
            }
            
            case 6: {
                printDistributionComparison();
                printGraphStatistics(graph);
                break;
            }
            
            case 0: {
                std::cout << "Exiting...\n";
                break;
            }
            
            default: {
                std::cout << "[!] Invalid choice. Try again.\n";
            }
        }
    } while (choice != 0);
    
    return 0;
}
