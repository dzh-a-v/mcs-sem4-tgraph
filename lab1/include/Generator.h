#pragma once
#include "include/Graph.h"
#include <random>

namespace graph {

/*
 * =============================================================================
 * RANDOM ACYCLIC GRAPH GENERATOR
 * =============================================================================
 * 
 * Generates random acyclic graphs with weighted edges.
 * Vertex degrees follow binomial distribution B(n, p).
 * 
 * =============================================================================
 * BINOMIAL DISTRIBUTION (Algorithm 1 - Vadzinsky, Section 6.1)
 * =============================================================================
 * 
 * Inverse transform method for discrete random variables:
 * 
 *   1. Initialize: q = 1-p, c = p/q, p(0) = q^n
 *   2. Generate r ~ Uniform(0,1)
 *   3. Set x = 0
 *   4. r = r - p(x)
 *   5. If r < 0: return x
 *   6. x = x + 1
 *   7. p(x) = p(x-1) * c * (n+1-x) / x   [recurrence formula]
 *   8. Go to step 4
 * 
 * Recurrence relation (Section 8.1):
 *   p(x) = p(x-1) × (p/q) × (n+1-x)/x
 * where p(0) = q^n
 * 
 * Distribution parameters (constants):
 *   BINOMIAL_N - number of trials
 *   BINOMIAL_P - success probability
 * 
 * =============================================================================
 * ACYCLICITY GUARANTEE
 * =============================================================================
 * 
 * For directed graphs, acyclicity is guaranteed by construction:
 *   - Only edges (u, v) where u < v are allowed
 *   - This creates a topological ordering: 0 → 1 → 2 → ... → n-1
 *   - No back-edges exist, therefore no cycles possible
 * 
 * For undirected graphs:
 *   - Edges are normalized so u < v (canonical representation)
 *   - Prevents duplicate edges between same vertex pair
 * 
 * =============================================================================
 * WEIGHT DISTRIBUTIONS
 * =============================================================================
 * 
 * Edge weights can be:
 *   - Positive only (Rayleigh distribution)
 *   - Negative only (-Rayleigh)
 *   - Mixed (signed Rayleigh)
 * =============================================================================
 */

// Binomial distribution parameters (constants)
constexpr int BINOMIAL_N = 10;       // Number of trials
constexpr double BINOMIAL_P = 0.3;   // Success probability

/// Theoretical characteristics of binomial distribution B(n, p)
struct BinomialCharacteristics {
    double mean;           // np
    double variance;       // npq
    double stdDev;         // sqrt(npq)
    int mode;              // floor((n+1)p)
    double skewness;       // (q-p)/sqrt(npq)
    double kurtosis;       // (1-6pq)/(npq)
};

/// Statistics computed from actual graph
struct GraphDegreeStatistics {
    double meanDegree;
    double variance;
    double stdDev;
    int minDegree;
    int maxDegree;
};

enum class WeightType {
    Positive,
    Negative,
    Mixed
};

/// Generates random acyclic graphs with weighted edges.
/// Vertex degrees follow binomial distribution B(BINOMIAL_N, BINOMIAL_P).
class Generator {
public:
    std::unique_ptr<Graph> generateAcyclicGraph(int vertices, int edges, 
        bool directed, WeightType weightType = WeightType::Positive);
    
    /// Computes theoretical characteristics of binomial distribution
    static BinomialCharacteristics getTheoreticalCharacteristics(int n, double p);
    
    /// Computes actual degree statistics from generated graph
    static GraphDegreeStatistics computeGraphStatistics(const Graph& graph);
    
private:
    std::mt19937 m_rng{std::random_device{}()};
    
    /// Generates binomial random number using Algorithm 1 (Vadzinsky)
    int generateBinomial(int n, double p);
    
    /// Generates edge weight based on weight type
    double generateWeight(WeightType weightType);
};
}