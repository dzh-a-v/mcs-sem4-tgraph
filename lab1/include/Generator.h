#pragma once
#include "include/Graph.h"
#include <random>

namespace graph {

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
