#pragma once
#include "include/Graph.h"
#include <random>

/*
 * =============================================================================
 * ACYCLIC GRAPH GENERATOR WITH BINOMIAL DISTRIBUTION
 * =============================================================================
 * 
 * Generates random acyclic graphs where:
 * - Vertex degrees follow binomial distribution B(n, p)
 * - Edge weights follow binomial distribution B(n, p)
 * 
 * Algorithm: Inverse transform method (Vadzinsky, Section 6.1)
 *   1. Initialize: q=1-p, c=p/q, prob=q^n
 *   2. Generate r ~ Uniform(0,1)
 *   3. Subtract probabilities until r < 0
 *   4. Return current x value
 * 
 * Acyclicity: Guaranteed by u < v constraint (topological order)
 * =============================================================================
 */

// Binomial parameters for vertex degrees
constexpr int BINOMIAL_N_DEGREE = 10;
constexpr double BINOMIAL_P_DEGREE = 0.3;

// Binomial parameters for edge weights
constexpr int BINOMIAL_N_WEIGHT = 10;
constexpr double BINOMIAL_P_WEIGHT = 0.5;

/// Theoretical properties of binomial distribution
struct BinomialProperties {
    double mean;
    double variance;
    double stdDev;
    int mode;
    double skewness;
    double kurtosis;
};

/// Actual graph statistics
struct DegreeStatistics {
    double meanDegree;
    double variance;
    double stdDev;
    int minDegree;
    int maxDegree;
};

/// Weight sign type
enum class WeightSign {
    Positive,
    Negative,
    Mixed
};

/// Random graph generator using binomial distribution
class AcyclicGraphBuilder {
public:
    /// Generate acyclic graph
    std::unique_ptr<AdjacencyGraph> generateAcyclicGraph(
        int vertices,
        int edges,
        bool directed,
        WeightSign weightSign = WeightSign::Positive);

    /// Get theoretical binomial characteristics
    static BinomialProperties getBinomialProperties(int n, double p);

    /// Compute actual graph statistics
    static DegreeStatistics computeDegreeStatistics(const AdjacencyGraph& graph);

    /// Generate binomial random value (Algorithm 1)
    int sampleBinomial(int n, double p);

private:
    std::mt19937 m_rng{std::random_device{}()};

    /// Generate edge weight
    double sampleWeight(WeightSign weightSign);
};
