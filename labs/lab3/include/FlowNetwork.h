#pragma once

#include <optional>
#include <ostream>
#include <unordered_map>
#include <vector>

// one real edge in flow network
struct FlowEdge {
    int from;
    int to;
    int capacity; // how much can pass through this edge at max
    int cost; // cost of sending 1 unit of flow through this edge
    int flow; // how much flow is currently pushed through this edge
};

// edge from residual network, not from the original one
// needed because max-flow / min-cost-flow work with "what can still be changed"
struct ResidualArc {
    int from;
    int to;
    int edgeIndex; // refers to the original edge from FlowEdge from m_edges
    bool forward; // true: forward through original edge;
                  // false: back (opportunity to reduce already pushed flow)
    int residualCapacity; // if it's forward: cap - flow
                          // if it's back: flow
    int cost; // forward: just cost
              // back: negative (we're taking flow back)
};

/*
Type for:
    - capacity matrix
    - cost matrix
    - flow matrix
'optional' because there might be no edge between two vertices.
    if exists: num
    if not: std::nullopt
why this and not 0:
    - 0 might be normal value
    - I used it before to show the absence of an edge
*/
using FlowMatrix = std::vector<std::vector<std::optional<int>>>;

class FlowNetwork {
public:
    explicit FlowNetwork(bool directed = true);

    void addVertex(int id);
    bool addEdge(int from, int to, int capacity, int cost);

    bool hasVertex(int id) const;
    bool isDirected() const;
    int size() const;

    std::vector<int> vertexIds() const;
    std::vector<FlowEdge> edges() const;
    // build neighbors for residual network "on the fly"
    // forward residual edge: we still can push cap - flow
    // backward residual edge: we can take already pushed flow back
    std::vector<ResidualArc> residualNeighbors(int vertex) const;

    // accessors for one real edge
    std::optional<FlowEdge> getEdge(int from, int to) const;
    std::optional<int> getCapacity(int from, int to) const;
    std::optional<int> getCost(int from, int to) const;
    std::optional<int> getFlow(int from, int to) const;

    // make matrix views for printing
    FlowMatrix getCapacityMatrix() const;
    FlowMatrix getCostMatrix() const;
    FlowMatrix getFlowMatrix() const;

    void resetFlows(); // set all current flows to 0
    /*
    update real edge using residual move
        if forward: flow += delta
        if back:    flot -= delta
    generally connects the res. arcs logic to flows of real arcs
    */
    void augment(int edgeIndex, bool forward, int delta); 


private:
    std::optional<int> findEdgeIndex(int from, int to) const; // find real edge in m_edges
    int indexForVertex(int id) const; // for matrices

    bool m_directed;
    std::vector<int> m_vertices; // list of ids as they were added
    std::unordered_map<int, int> m_idToIndex; // id -> position in m_vertices / matrices
    std::vector<FlowEdge> m_edges; // all real edges live here
    std::unordered_map<int, std::vector<int>> m_outgoing; // vertex -> indexes of outgoing edges in m_edges
    std::unordered_map<int, std::vector<int>> m_incoming; // vertex -> indexes of incoming edges in m_edges
};

void printFlowMatrix(std::ostream& out, const FlowMatrix& matrix,
                     const std::vector<int>& vertexIds);
