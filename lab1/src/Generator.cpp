#include "include/Generator.h"
#include <cmath>
#include <random>
#include <set>
#include <numeric>
#include <algorithm>
#include <vector>
#include <iostream>

namespace tgraph {

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
static bool checkConnected(const AdjacencyGraph& graph, int vertexCount) {
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
        for (const auto& [neighbor, weight] : graph.neighbors(current)) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                visitedCount++;
                queue.push_back(neighbor);
            }
        }
    }
    
    return visitedCount == vertexCount;
}

// Try to build graph with given degree targets
static std::unique_ptr<AdjacencyGraph> tryBuildGraph(
    int vertexCount,
    int targetEdges,
    bool directed,
    WeightSign weightSign,
    const std::vector<int>& degreeTargets,
    std::mt19937& rng)
{
    auto graph = std::make_unique<AdjacencyGraph>(directed);
    
    for (int i = 0; i < vertexCount; ++i) {
        graph->addVertex(i);
    }
    
    std::vector<int> currentDegree(vertexCount, 0);
    std::set<std::pair<int, int>> addedEdges;
    
    int edgesAdded = 0;
    int attempts = 0;
    const int maxAttempts = targetEdges * 100;
    
    while (edgesAdded < targetEdges && attempts < maxAttempts) {
        ++attempts;
        
        int u = std::uniform_int_distribution<>(0, vertexCount - 2)(rng);
        int v = std::uniform_int_distribution<>(u + 1, vertexCount - 1)(rng);
        
        if (addedEdges.count({u, v})) continue;
        
        if (degreeTargets[u] > 0 && currentDegree[u] >= degreeTargets[u]) continue;
        if (degreeTargets[v] > 0 && currentDegree[v] >= degreeTargets[v]) continue;
        
        // Generate weight using binomial
        int baseWeight = sampleBinomial(BINOMIAL_N_WEIGHT, BINOMIAL_P_WEIGHT);
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
    
    return graph;
}

std::unique_ptr<AdjacencyGraph> AcyclicGraphBuilder::generateAcyclicGraph(
    int vertexCount, 
    int targetEdges, 
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
        auto graph = tryBuildGraph(vertexCount, targetEdges, directed, weightSign, 
                                   degreeTargets, m_rng);
        
        // Step 3: Check if connected
        if (checkConnected(*graph, vertexCount)) {
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
    
    return tryBuildGraph(vertexCount, targetEdges, directed, weightSign, 
                         degreeTargets, m_rng);
}
}
