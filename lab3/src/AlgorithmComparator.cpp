#include "include/AlgorithmComparator.h"

AlgorithmComparator::AlgorithmComparator(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

AlgorithmComparison AlgorithmComparator::compare(int startVertex) {
    AlgorithmComparison result;
    
    // Run BFS
    BreadthFirstSearch bfs(m_graph);
    BFSResult bfsResult = bfs.traverseWithLevels(startVertex);
    result.bfsIterations = bfsResult.iterations;
    
    // Run Dijkstra
    DijkstraAlgorithm dijkstra(m_graph);
    DijkstraResult dijkstraResult = dijkstra.findShortestPaths(startVertex);
    result.dijkstraIterations = dijkstraResult.iterations;
    
    // Calculate speedup (BFS is typically faster for unweighted traversal)
    if (result.dijkstraIterations > 0) {
        result.speedupFactor = static_cast<double>(result.dijkstraIterations) / 
                               static_cast<double>(result.bfsIterations);
    } else {
        result.speedupFactor = 0.0;
    }
    
    return result;
}
