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
 * - Vertex degrees follow binomial distribution B(n, p)
 * - Edge weights also follow binomial distribution (shifted to avoid zero)
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
 *   BINOMIAL_N_DEGREE - number of trials for vertex degrees
 *   BINOMIAL_P_DEGREE - success probability for vertex degrees
 *   BINOMIAL_N_WEIGHT - number of trials for edge weights
 *   BINOMIAL_P_WEIGHT - success probability for edge weights
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
 * WEIGHT TYPES
 * =============================================================================
 * 
 * Edge weights can be:
 *   - Positive only: use binomial directly (values 1 to n)
 *   - Negative only: negate binomial (values -n to -1)
 *   - Mixed: randomly positive or negative binomial
 * =============================================================================
 */

// Binomial distribution parameters for vertex degrees
constexpr int BINOMIAL_N_DEGREE = 10;    // Number of trials for degrees
constexpr double BINOMIAL_P_DEGREE = 0.3; // Success probability for degrees

// Binomial distribution parameters for edge weights
constexpr int BINOMIAL_N_WEIGHT = 10;    // Number of trials for weights
constexpr double BINOMIAL_P_WEIGHT = 0.3; // Success probability for weights

/// Theoretical characteristics
struct BinomialCharacteristics {
    double mean;           
    double variance;       
    double stdDev;
    int mode;  
    double skewness;
    double kurtosis;
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


class Generator {
public:
    std::unique_ptr<Graph> generateAcyclicGraph(int vertices, int edges,
        bool directed, WeightType weightType = WeightType::Positive);  
    static BinomialCharacteristics getTheoreticalCharacteristics(int n, double p);
    static GraphDegreeStatistics computeGraphStatistics(const Graph& graph);

private:
    std::mt19937 m_rng{std::random_device{}()};

    /// Generates binomial random number using Algorithm 1 (Vadzinsky)
    int generateBinomial(int n, double p);

    /// Generates edge weight based on weight type using binomial distribution
    double generateWeight(WeightType weightType);
};
}
