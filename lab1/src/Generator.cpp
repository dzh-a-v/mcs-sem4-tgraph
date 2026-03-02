#include "include/Generator.h"
#include <cmath>
#include <random>
#include <set>
#include <numeric>
#include <algorithm>
#include <vector>

namespace graph {

BinomialCharacteristics Generator::getTheoreticalCharacteristics(int n, double p) {
    double q = 1.0 - p;
    double np1 = (n + 1) * p;
    
    BinomialCharacteristics chars;
    chars.mean = n * p;
    chars.variance = n * p * q;
    chars.stdDev = std::sqrt(chars.variance);
    
    // Mode formula from Section 1.6
    double intpart;
    if (std::modf(np1, &intpart) == 0.0) {
        // (n+1)p is integer - two modes
        chars.mode = static_cast<int>(np1);  // Use the higher mode
    } else {
        // (n+1)p is not integer - floor
        chars.mode = static_cast<int>(np1);
    }
    
    chars.skewness = (q - p) / std::sqrt(n * p * q);
    chars.kurtosis = (1.0 - 6.0 * p * q) / (n * p * q);
    
    return chars;
}

GraphDegreeStatistics Generator::computeGraphStatistics(const Graph& graph) {
    GraphDegreeStatistics stats;
    
    auto vertices = graph.vertexIds();
    int n = static_cast<int>(vertices.size());
    
    if (n == 0) {
        stats.meanDegree = 0;
        stats.variance = 0;
        stats.stdDev = 0;
        stats.minDegree = 0;
        stats.maxDegree = 0;
        return stats;
    }
    
    // Compute degree for each vertex
    std::vector<int> degrees;
    degrees.reserve(n);
    
    for (int v : vertices) {
        degrees.push_back(static_cast<int>(graph.neighbors(v).size()));
    }
    
    // Mean
    double sum = std::accumulate(degrees.begin(), degrees.end(), 0.0);
    stats.meanDegree = sum / n;
    
    // Variance
    double sqSum = 0.0;
    for (int d : degrees) {
        sqSum += (d - stats.meanDegree) * (d - stats.meanDegree);
    }
    stats.variance = sqSum / n;
    stats.stdDev = std::sqrt(stats.variance);
    
    // Min/Max
    auto [minIt, maxIt] = std::minmax_element(degrees.begin(), degrees.end());
    stats.minDegree = *minIt;
    stats.maxDegree = *maxIt;
    
    return stats;
}

int Generator::generateBinomial(int n, double p) {
    // Algorithm 1 from Vadzinsky reference (Section 6.1)
    // Uses inverse transform method with recurrence relation
    
    double q = 1.0 - p;
    double c = p / q;
    double prob = std::pow(q, n);  // p(0) = q^n
    
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    double r = uniform(m_rng);
    int x = 0;
    
    while (true) {
        r -= prob;
        if (r < 0.0) {
            return x;
        }
        
        x++;
        // Recurrence: p(x) = p(x-1) * c * (n+1-x) / x
        prob *= c * (n + 1 - x) / x;
    }
}

double Generator::generateWeight(WeightType weightType) {
    // Generate weight using binomial distribution (Algorithm 1)
    // Add 1 to avoid zero weights (binomial can return 0)
    int weight = generateBinomial(BINOMIAL_N_WEIGHT, BINOMIAL_P_WEIGHT) + 1;
    
    switch (weightType) {
        case WeightType::Positive:
            return static_cast<double>(weight);
        case WeightType::Negative:
            return -static_cast<double>(weight);
        case WeightType::Mixed: {
            std::uniform_int_distribution<int> sign(0, 1);
            return sign(m_rng) ? static_cast<double>(weight) : -static_cast<double>(weight);
        }
    }
    return static_cast<double>(weight);
}

std::unique_ptr<Graph> Generator::generateAcyclicGraph(int vertices, int edges, bool directed, WeightType weightType) {
    auto graph = std::make_unique<Graph>(directed);
    
    // Add all vertices
    for (int i = 0; i < vertices; ++i) {
        graph->addVertex(i);
    }
    
    // Generate target degrees for each vertex using binomial distribution
    std::vector<int> targetDegrees(vertices);
    int totalDegree = 0;
    for (int i = 0; i < vertices; ++i) {
        // Limit degree to avoid impossible situations
        targetDegrees[i] = std::min(generateBinomial(BINOMIAL_N_DEGREE, BINOMIAL_P_DEGREE), vertices - 1);
        totalDegree += targetDegrees[i];
    }
    
    // For undirected graphs, total degree must be even (handshaking lemma)
    if (!directed && totalDegree % 2 != 0) {
        // Adjust one vertex degree to make sum even
        for (int i = 0; i < vertices && totalDegree % 2 != 0; ++i) {
            if (targetDegrees[i] > 0) {
                targetDegrees[i]--;
                totalDegree--;
            }
        }
    }
    
    // Calculate max possible edges for acyclic graph
    int maxEdges = directed ? (vertices * (vertices - 1)) / 2 : (vertices * (vertices - 1)) / 2;
    int actualEdges = std::min(edges, maxEdges);
    
    // Track current degree of each vertex
    std::vector<int> currentDegree(vertices, 0);
    std::set<std::pair<int, int>> existingEdges;
    
    int added = 0;
    int attempts = 0;
    const int maxAttempts = actualEdges * 100;
    
    while (added < actualEdges && attempts < maxAttempts) {
        attempts++;
        
        // Select two vertices u < v (ensures acyclicity for directed)
        int u = std::uniform_int_distribution<>(0, vertices - 2)(m_rng);
        int v = std::uniform_int_distribution<>(u + 1, vertices - 1)(m_rng);
        
        // Check if edge already exists
        if (existingEdges.find({u, v}) != existingEdges.end()) {
            continue;
        }
        
        // Check degree constraints
        int maxDegreeU = directed ? vertices - 1 - u : vertices - 1;
        int maxDegreeV = directed ? v : vertices - 1;
        
        if (currentDegree[u] >= targetDegrees[u] && targetDegrees[u] > 0) {
            continue;
        }
        if (currentDegree[v] >= targetDegrees[v] && targetDegrees[v] > 0) {
            continue;
        }
        
        // Add edge with binomial-distributed weight
        graph->addEdge(u, v, generateWeight(weightType));
        existingEdges.insert({u, v});
        currentDegree[u]++;
        currentDegree[v]++;
        added++;
    }
    
    // Ensure connectivity: if graph is not connected, add minimum spanning edges
    // For acyclic directed graph: ensure path 0 -> 1 -> 2 -> ... -> n-1 exists
    std::vector<bool> visited(vertices, false);
    std::vector<int> queue;
    
    if (vertices > 0) {
        queue.push_back(0);
        visited[0] = true;
        
        size_t head = 0;
        while (head < queue.size()) {
            int curr = queue[head++];
            for (auto const& [neighbor, _] : graph->neighbors(curr)) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    queue.push_back(neighbor);
                }
            }
        }
    }
    
    // Connect unvisited vertices
    for (int i = 1; i < vertices; ++i) {
        if (!visited[i]) {
            // Add edge from i-1 to i to ensure connectivity
            if (existingEdges.find({i - 1, i}) == existingEdges.end()) {
                graph->addEdge(i - 1, i, generateWeight(weightType));
                existingEdges.insert({i - 1, i});
            }
            visited[i] = true;
        }
    }
    
    return graph;
}
}
