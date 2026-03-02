#pragma once
#include "include/Graph.h"
#include <random>

constexpr int BINOMIAL_N_DEGREE = 10;
constexpr double BINOMIAL_P_DEGREE = 0.3;

constexpr int BINOMIAL_N_WEIGHT = 10;
constexpr double BINOMIAL_P_WEIGHT = 0.3;

struct BinomialProperties {
    double mean;
    double variance;
    double stdDev;
    int mode;
    double skewness;
    double kurtosis;
};

struct DegreeStatistics {
    double meanDegree;
    double variance;
    double stdDev;
    int minDegree;
    int maxDegree;
};

enum class WeightSign {
    Positive,
    Negative,
    Mixed
};

class AcyclicGraphBuilder {
public:
    std::unique_ptr<AdjacencyGraph> generateAcyclicGraph(
        int vertices,
        int edges,
        bool directed,
        WeightSign weightSign = WeightSign::Positive);

    static BinomialProperties getBinomialProperties(int n, double p);

    static DegreeStatistics computeDegreeStatistics(const AdjacencyGraph& graph);

    int sampleBinomial(int n, double p);

private:
    std::mt19937 m_rng{std::random_device{}()};

    double sampleWeight(WeightSign weightSign);
};
