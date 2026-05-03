#include "include/PruferCode.h"
#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>

namespace {

// Helper: find the leaf with the smallest ID in `currentDegree` over `aliveVertices`.
// Returns the vertex ID (caller is sure at least one leaf exists in a non-trivial tree).
int findSmallestLeaf(
    const std::vector<int>& aliveVertices,
    const std::unordered_map<int, int>& currentDegree)
{
    int smallest = -1;
    for (int v : aliveVertices) {
        if (currentDegree.at(v) == 1) {
            if (smallest == -1 || v < smallest) {
                smallest = v;
            }
        }
    }
    return smallest;
}

}  // namespace

PruferCode PruferEncoder::encode(const AdjacencyGraph& tree) {
    PruferCode result;

    const int p = tree.size();
    if (p < 2) {
        // No edges to record. By the slide-30 formula the code length is p-1,
        // which is 0 for p=1 and -1 for p=0 -- we simply return empty.
        return result;
    }

    // Build a working representation we can mutate as we strip leaves:
    //   - aliveVertices: the set of IDs still present in the tree
    //   - adjacency:     a multiset-like adjacency map (vertex -> neighbors)
    //   - edgeWeight:    weight of an unordered edge {u, v}, looked up by
    //                    (min(u,v), max(u,v)) so direction doesn't matter
    //   - currentDegree: degree of each vertex in the shrinking tree
    //
    // We don't mutate the input graph -- everything happens in these local
    // structures.

    std::vector<int> aliveVertices = tree.vertexIds();

    std::unordered_map<int, std::unordered_set<int>> adjacency;
    std::unordered_map<int, int> currentDegree;
    for (int v : aliveVertices) {
        adjacency[v] = {};
        currentDegree[v] = 0;
    }

    // edges() returns each undirected edge once (filtered by from < to internally),
    // so we just record both directions and the weight once.
    auto edgeKey = [](int a, int b) {
        if (a > b) std::swap(a, b);
        return std::pair<int, int>{a, b};
    };

    std::map<std::pair<int, int>, double> edgeWeight;
    for (const WeightedEdge& edge : tree.edges()) {
        adjacency[edge.from].insert(edge.to);
        adjacency[edge.to].insert(edge.from);
        currentDegree[edge.from] += 1;
        currentDegree[edge.to] += 1;
        edgeWeight[edgeKey(edge.from, edge.to)] = edge.weight;
    }

    // Main loop: exactly p - 1 iterations, per slide 30.
    // Each iteration removes the smallest-ID leaf and writes down (neighbor, weight).
    result.code.reserve(p - 1);
    result.weights.reserve(p - 1);

    for (int i = 0; i < p - 1; ++i) {
        // Step 1: find leaf v with smallest ID (degree == 1).
        const int leaf = findSmallestLeaf(aliveVertices, currentDegree);
        if (leaf == -1) {
            // Shouldn't happen in a valid tree, but bail out gracefully.
            break;
        }

        // Step 2: identify v's only neighbor.
        // adjacency[leaf] has exactly one element since deg(leaf) == 1.
        const int neighbor = *adjacency[leaf].begin();

        // Step 3: write code[i] = neighbor, weights[i] = w(leaf, neighbor).
        result.code.push_back(neighbor);
        result.weights.push_back(edgeWeight.at(edgeKey(leaf, neighbor)));

        // Step 4: physically remove leaf from the tree.
        // - drop the edge from both adjacency lists
        // - decrement the neighbor's degree
        // - remove leaf from aliveVertices
        adjacency[neighbor].erase(leaf);
        adjacency[leaf].clear();
        currentDegree[neighbor] -= 1;
        currentDegree[leaf] = 0;

        aliveVertices.erase(
            std::find(aliveVertices.begin(), aliveVertices.end(), leaf));
    }

    return result;
}

std::unique_ptr<AdjacencyGraph> PruferEncoder::decode(
    const PruferCode& pruferCode,
    const std::vector<int>& vertexIds)
{
    auto tree = std::make_unique<AdjacencyGraph>(false);  // free tree -> undirected

    // Always add all vertices, even if some never appear in the code.
    for (int v : vertexIds) {
        tree->addVertex(v);
    }

    const int p = static_cast<int>(vertexIds.size());
    if (p < 2) {
        return tree;  // 0 or 1 vertices -> no edges to add
    }

    // Sanity: the code must have length p - 1 to match our encoder.
    // If it doesn't, decode whatever we can without crashing.
    const int codeLength = static_cast<int>(pruferCode.code.size());
    const int weightsLength = static_cast<int>(pruferCode.weights.size());
    const int steps = std::min({p - 1, codeLength, weightsLength});

    // B = unused vertex IDs (sorted ascending so "smallest unused" is just front()).
    std::vector<int> unusedSorted = vertexIds;
    std::sort(unusedSorted.begin(), unusedSorted.end());

    // Decoder per slide 31:
    //   for i from 1 to p - 1:
    //     v := min { k in B | for all j >= i, k != A[j] }
    //     E := E + (v, A[i])
    //     B := B - v
    //
    // "Smallest unused ID that does NOT appear in the remaining suffix of the code".
    for (int i = 0; i < steps; ++i) {
        // Build the set of values present in code[i..end]: that's the forbidden set.
        std::unordered_set<int> remainingInCode;
        for (int j = i; j < codeLength; ++j) {
            remainingInCode.insert(pruferCode.code[j]);
        }

        // Walk unusedSorted from the front to find the smallest k not in remainingInCode.
        int chosenIdx = -1;
        for (int idx = 0; idx < static_cast<int>(unusedSorted.size()); ++idx) {
            if (remainingInCode.find(unusedSorted[idx]) == remainingInCode.end()) {
                chosenIdx = idx;
                break;
            }
        }
        if (chosenIdx == -1) {
            // Malformed code -- the suffix forbids every remaining vertex.
            // Stop gracefully rather than throw.
            break;
        }

        const int v = unusedSorted[chosenIdx];
        const int neighbor = pruferCode.code[i];
        const double weight = pruferCode.weights[i];

        tree->addEdge(v, neighbor, weight);

        // Remove v from unused.
        unusedSorted.erase(unusedSorted.begin() + chosenIdx);
    }

    // After p - 1 steps the encoder leaves exactly one vertex in B and one
    // unrecorded edge (the last leaf attached to the last survivor). Per slide 30
    // we DO write down that final edge in the code, so the loop above already
    // covered all p - 1 edges -- no extra "join the last two unused" step needed.

    return tree;
}
