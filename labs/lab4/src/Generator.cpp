#include "include/Generator.h"
#include <cmath>
#include <random>
#include <set>
#include <numeric>
#include <algorithm>
#include <vector>
#include <iostream>

BinomialProperties AcyclicGraphBuilder::getBinomialProperties(int n, double p) {
    double q = 1.0 - p;
    double np1 = (n + 1) * p;
    
    BinomialProperties props{};
    props.mean = n * p;
    props.variance = n * p * q;
    props.stdDev = std::sqrt(props.variance);
    
    props.mode = static_cast<int>(np1);
    props.skewness = (q - p) / std::sqrt(n * p * q);
    props.kurtosis = (1.0 - 6.0 * p * q) / (n * p * q);
    
    return props;
}
// ребра не вводим в инпут, они генерируются на основе степеней автоматически

DegreeStatistics AcyclicGraphBuilder::computeDegreeStatistics(const AdjacencyGraph& graph) {
    DegreeStatistics stats{};
    auto vertices = graph.vertexIds();
    int vertexCount = static_cast<int>(vertices.size());
    
    if (vertexCount == 0) {
        return stats;
    }
    
    std::vector<int> degreeList;
    degreeList.reserve(vertexCount);
    
    for (int v : vertices) {
        degreeList.push_back(static_cast<int>(graph.neighbors(v).size()));
    }
    
    double total = std::accumulate(degreeList.begin(), degreeList.end(), 0.0);
    stats.meanDegree = total / vertexCount;
    
    double sqDiffSum = 0.0;
    for (int deg : degreeList) {
        sqDiffSum += (deg - stats.meanDegree) * (deg - stats.meanDegree);
    }
    stats.variance = sqDiffSum / vertexCount;
    stats.stdDev = std::sqrt(stats.variance);
    
    auto result = std::minmax_element(degreeList.begin(), degreeList.end());
    stats.minDegree = *result.first;
    stats.maxDegree = *result.second;
    
    return stats;
}

int AcyclicGraphBuilder::sampleBinomial(int n, double p) {
    double q = 1.0 - p;
    double ratio = p / q;
    double probability = std::pow(q, n);
    
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    double randomVal = uniform(m_rng);
    int x = 0;
    
    while (true) {
        randomVal -= probability;
        if (randomVal < 0.0) {
            return x;
        }
        
        ++x;
        probability *= ratio * (n + 1 - x) / x;
    }
}

double AcyclicGraphBuilder::sampleWeight(WeightSign weightSign) {
    int baseWeight = sampleBinomial(BINOMIAL_N_WEIGHT, BINOMIAL_P_WEIGHT);
    
    switch (weightSign) {
        case WeightSign::Positive:
            return static_cast<double>(baseWeight);
        case WeightSign::Negative:
            return -static_cast<double>(baseWeight);
        case WeightSign::Mixed: {
            std::uniform_int_distribution<int> coinFlip(0, 1);
            return coinFlip(m_rng) ? 
                static_cast<double>(baseWeight) : 
                -static_cast<double>(baseWeight);
        }
    }
    return static_cast<double>(baseWeight);
}

// Check if graph is connected using BFS
// For directed graphs: use WEAK connectivity (ignore edge direction)
// For undirected: standard connectivity
static bool checkConnected(const AdjacencyGraph& graph, int vertexCount, bool directed) {
    if (vertexCount <= 1) return true;
    
    std::vector<bool> visited(vertexCount, false);
    std::vector<int> queue;
    int visitedCount = 0;
    
    queue.push_back(0);
    visited[0] = true;
    visitedCount++;
    
    size_t head = 0;
    while (head < queue.size()) {
        int current = queue[head++];
        
        // For directed graphs, we need to check both outgoing AND incoming edges
        // (weak connectivity - treat as undirected)
        for (int other = 0; other < vertexCount; ++other) {
            if (visited[other]) continue;
            
            // Check if there's an edge either direction
            bool hasEdge = false;
            
            // Check outgoing from current
            for (const auto& [neighbor, weight] : graph.neighbors(current)) {
                if (neighbor == other) {
                    hasEdge = true;
                    break;
                }
            }
            
            // Check incoming to current (for directed graphs)
            if (!hasEdge && directed) {
                for (const auto& [neighbor, weight] : graph.neighbors(other)) {
                    if (neighbor == current) {
                        hasEdge = true;
                        break;
                    }
                }
            }
            
            if (hasEdge) {
                visited[other] = true;
                visitedCount++;
                queue.push_back(other);
            }
        }
    }
    
    return visitedCount == vertexCount;
}

// Try to build graph with given degree targets
static std::unique_ptr<AdjacencyGraph> tryBuildGraph(
    int vertexCount,
    bool directed,
    WeightSign weightSign,
    const std::vector<int>& degreeTargets,
    std::mt19937& rng,
    AcyclicGraphBuilder& builder)
{
    auto graph = std::make_unique<AdjacencyGraph>(directed);

    for (int i = 0; i < vertexCount; ++i) {
        graph->addVertex(i);
    }

    std::vector<int> currentDegree(vertexCount, 0);
    std::set<std::pair<int, int>> addedEdges;

    int edgesAdded = 0;

    // Calculate target edges from degree targets (sum of degrees / 2 for undirected)
    int sumDegrees = 0;
    for (int deg : degreeTargets) {
        sumDegrees += deg;
    }
    // For undirected: edges = sum(degrees) / 2
    // For directed: edges = sum(degrees) (each edge contributes to out-degree only)
    int targetEdges = directed ? sumDegrees : sumDegrees / 2;

    // Calculate max possible edges for acyclic graph (u < v constraint)
    int maxPossibleEdges = vertexCount * (vertexCount - 1) / 2;
    int actualTargetEdges = std::min(targetEdges, maxPossibleEdges);

    // Base attempts on actual possible edges, not requested edges
    int attempts = 0;
    const int maxAttempts = actualTargetEdges * 100 + 1000;

    while (edgesAdded < actualTargetEdges && attempts < maxAttempts) {
        ++attempts;

        int u = std::uniform_int_distribution<>(0, vertexCount - 2)(rng);
        int v = std::uniform_int_distribution<>(u + 1, vertexCount - 1)(rng);

        if (addedEdges.count({u, v})) continue;

        // Only check degree targets if they were set (> 0)
        // But don't let them block us if we still need more edges
        bool uBlocked = (degreeTargets[u] > 0 && currentDegree[u] >= degreeTargets[u]);
        bool vBlocked = (degreeTargets[v] > 0 && currentDegree[v] >= degreeTargets[v]);

        // Skip only if BOTH vertices are at their degree limit
        // This allows flexibility to reach targetEdges
        if (uBlocked && vBlocked) continue;

        // Generate weight using binomial
        int baseWeight = builder.sampleBinomial(BINOMIAL_N_WEIGHT, BINOMIAL_P_WEIGHT);
        double weight = static_cast<double>(baseWeight);

        if (weightSign == WeightSign::Negative) {
            weight = -weight;
        } else if (weightSign == WeightSign::Mixed) {
            std::uniform_int_distribution<int> sign(0, 1);
            if (sign(rng)) weight = -weight;
        }

        graph->addEdge(u, v, weight);
        addedEdges.insert({u, v});
        ++currentDegree[u];
        ++currentDegree[v];
        ++edgesAdded;
    }

    if (edgesAdded < actualTargetEdges) {
        std::cout << "[!] Warning: Could only add " << edgesAdded
                  << " edges (max possible for " << vertexCount
                  << " vertices: " << maxPossibleEdges << ")\n";
    }

    return graph;
}

std::unique_ptr<AdjacencyGraph> AcyclicGraphBuilder::generateAcyclicGraph(
    int vertexCount,
    bool directed,
    WeightSign weightSign)
{
    const int maxRegenerations = 50;

    for (int attempt = 1; attempt <= maxRegenerations; ++attempt) {
        // Step 1: Generate ALL vertex degrees using binomial distribution
        std::vector<int> degreeTargets(vertexCount);
        int totalDegree = 0;

        for (int i = 0; i < vertexCount; ++i) {
            degreeTargets[i] = std::min(
                sampleBinomial(BINOMIAL_N_DEGREE, BINOMIAL_P_DEGREE),
                vertexCount - 1
            );
            totalDegree += degreeTargets[i];
        }

        // Fix parity for undirected graphs (handshaking lemma)
        if (!directed && totalDegree % 2 != 0) {
            for (int i = 0; i < vertexCount && totalDegree % 2 != 0; ++i) {
                if (degreeTargets[i] > 0) {
                    --degreeTargets[i];
                    --totalDegree;
                }
            }
        }

        // Step 2: Build graph based on degree targets
        auto graph = tryBuildGraph(vertexCount, directed, weightSign,
                                   degreeTargets, m_rng, *this);

        // Step 3: Check if connected (weak connectivity for directed)
        if (checkConnected(*graph, vertexCount, directed)) {
            // Success! Return connected graph
            return graph;
        }

        // Step 4: Not connected → discard EVERYTHING and regenerate
        // (both degrees AND edges)
        // Loop continues to next attempt
    }

    // After max attempts, return last attempt
    std::cout << "[!] Warning: Graph may be disconnected after "
              << maxRegenerations << " regeneration attempts\n";
    std::cout << "    Try: more edges, or different binomial parameters (n, p)\n";

    // One final attempt
    std::vector<int> degreeTargets(vertexCount);
    for (int i = 0; i < vertexCount; ++i) {
        degreeTargets[i] = std::min(
            sampleBinomial(BINOMIAL_N_DEGREE, BINOMIAL_P_DEGREE),
            vertexCount - 1
        );
    }

    return tryBuildGraph(vertexCount, directed, weightSign,
                         degreeTargets, m_rng, *this);
}
