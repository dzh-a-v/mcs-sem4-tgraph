#pragma once
#include "include/Graph.h"
#include <random>

constexpr int BINOMIAL_N_DEGREE = 15;
constexpr double BINOMIAL_P_DEGREE = 0.5;

constexpr int BINOMIAL_N_WEIGHT = 15;
constexpr double BINOMIAL_P_WEIGHT = 0.5;

/* 
Я всё ещё путаюсь, что это вообще такое :Р поэтому это мне шпаргалка.
k=0:  ||     (2.8%)
k=1:  ||||||||  (12.1%)
k=2:  |||||||||||||| (23.3%)
k=3:  |||||||||||||||| (26.7%)
k=4:  ||||||||||||  (20.0%)
k=5:  |||||||    (10.3%)
k=6:  ||||      (3.7%)
k=7:  ||      (0.9%)
k=8:  |       (0.1%)
k=9:          (0.01%)
k=10:         (0.0006%)
*/

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
        bool directed,
        WeightSign weightSign = WeightSign::Positive);

    static BinomialProperties getBinomialProperties(int n, double p);

    static DegreeStatistics computeDegreeStatistics(const AdjacencyGraph& graph);

    int sampleBinomial(int n, double p);

private:
    std::mt19937 m_rng{std::random_device{}()};

    double sampleWeight(WeightSign weightSign);
};
