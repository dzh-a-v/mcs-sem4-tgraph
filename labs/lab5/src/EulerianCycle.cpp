#include "include/EulerianCycle.h"
#include <algorithm>
#include <functional>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>

EulerianCycleBuilder::EulerianCycleBuilder(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

namespace {

// ? исходный m_graph не меняем — все правки только в копии
std::unique_ptr<AdjacencyGraph> cloneGraph(const AdjacencyGraph& g) {
    auto copy = std::make_unique<AdjacencyGraph>(g.isDirected());
    for (int v : g.vertexIds()) {
        copy->addVertex(v);
    }
    for (const WeightedEdge& e : g.edges()) {
        copy->addEdge(e.from, e.to, e.weight);
    }
    return copy;
}

// Degree = length of adjacency list.
int degreeOf(const AdjacencyGraph& g, int v) {
    return static_cast<int>(g.neighbors(v).size());
}

// ? связность только among вершин с рёбрами; изолированные не тянут мосты
std::vector<std::vector<int>> connectedComponentsWithEdges(const AdjacencyGraph& g) {
    std::vector<std::vector<int>> components;
    std::unordered_set<int> visited;

    for (int start : g.vertexIds()) {
        if (visited.count(start)) continue;
        if (g.neighbors(start).empty()) continue;

        std::vector<int> component;
        std::vector<int> queue{start};
        visited.insert(start);
        size_t head = 0;
        while (head < queue.size()) {
            int u = queue[head++];
            component.push_back(u);
            for (const auto& [w, _wt] : g.neighbors(u)) {
                if (!visited.count(w)) {
                    visited.insert(w);
                    queue.push_back(w);
                }
            }
        }
        components.push_back(std::move(component));
    }
    return components;
}

// Hierholzer: stack walk, pop vertices when stuck. Even degrees -> cycle;
// two odd vertices and start at one of them -> Eulerian path.
std::vector<int> hierholzer(const AdjacencyGraph& g, int start) {
    // One id per undirected edge so both endpoints share the same "used" flag.
    struct AdjSlot {  // ? сосед + id ребра
        int neighbor;
        int edgeId;
    };

    std::unordered_map<int, std::vector<AdjSlot>> adj;
    int nextId = 0;

    for (const WeightedEdge& e : g.edges()) {
        const int id = nextId++;
        adj[e.from].push_back({e.to, id});
        adj[e.to].push_back({e.from, id});
    }

    std::unordered_set<int> usedEdges;
    std::unordered_map<int, size_t> cursor;

    std::stack<int> path;
    std::vector<int> trail;

    path.push(start);  // ? старт Иерихольцера: отсюда зависит цикл vs цепь

    while (!path.empty()) {
        int u = path.top();
        auto& list = adj[u];
        size_t& i = cursor[u];

        while (i < list.size() && usedEdges.count(list[i].edgeId)) {
            ++i;
        }

        if (i == list.size()) {
            trail.push_back(u);
            path.pop();
        } else {
            const AdjSlot slot = list[i];
            usedEdges.insert(slot.edgeId);  // ? каждое ребро графа — один раз
            ++i;
            path.push(slot.neighbor);
        }
    }

    std::reverse(trail.begin(), trail.end());
    return trail;
}

// Existing edges as normalized pairs (u < v).
std::set<std::pair<int,int>> existingEdgePairs(const AdjacencyGraph& g) {
    std::set<std::pair<int,int>> pairs;
    for (const WeightedEdge& e : g.edges()) {
        int a = e.from, b = e.to;
        if (a > b) std::swap(a, b);
        pairs.insert({a, b});
    }
    return pairs;
}

// ? эйлеризация: пары нечётных, но не по существующему ребру
bool findNonMultigraphMatching(const std::vector<int>& oddVertices,
                               const std::set<std::pair<int,int>>& forbidden,
                               std::vector<std::pair<int,int>>& pairs) {
    const size_t n = oddVertices.size();
    std::vector<bool> used(n, false);

    auto isForbidden = [&](int u, int v) {
        int a = u, b = v;
        if (a > b) std::swap(a, b);
        return forbidden.count({a, b}) > 0;
    };

    std::function<bool()> recurse = [&]() -> bool {
        size_t i = 0;
        while (i < n && used[i]) ++i;
        if (i == n) return true;  // all matched

        used[i] = true;
        for (size_t j = i + 1; j < n; ++j) {
            if (used[j]) continue;
            if (isForbidden(oddVertices[i], oddVertices[j])) continue;

            used[j] = true;
            pairs.push_back({oddVertices[i], oddVertices[j]});
            if (recurse()) return true;
            pairs.pop_back();
            used[j] = false;
        }
        used[i] = false;
        return false;
    };

    pairs.clear();
    return recurse();
}

bool canRemoveEdgeWithoutDisconnecting(const AdjacencyGraph& g, int u, int v) {
    auto copy = cloneGraph(g);
    if (!copy->removeEdge(u, v).has_value()) {
        return false;
    }

    if (copy->edges().empty()) {
        return false;
    }

    return connectedComponentsWithEdges(*copy).size() <= 1;
}

bool findDeletionMatching(AdjacencyGraph& graph,
                          const std::vector<int>& oddVertices,
                          std::vector<std::pair<int,int>>& deletions) {
    const size_t n = oddVertices.size();
    std::vector<bool> used(n, false);

    std::function<bool()> recurse = [&]() -> bool {
        size_t i = 0;
        while (i < n && used[i]) ++i;
        if (i == n) return true;

        used[i] = true;
        for (size_t j = i + 1; j < n; ++j) {
            if (used[j]) continue;

            const int u = oddVertices[i];
            const int v = oddVertices[j];
            if (!graph.getEdgeWeight(u, v).has_value()) continue;
            if (!canRemoveEdgeWithoutDisconnecting(graph, u, v)) continue;

            const auto removedWeight = graph.removeEdge(u, v);  // ? удаление ребра в G
            if (!removedWeight.has_value()) continue;

            used[j] = true;
            deletions.push_back({u, v});
            if (recurse()) return true;

            deletions.pop_back();
            used[j] = false;
            graph.addEdge(u, v, *removedWeight);
        }

        used[i] = false;
        return false;
    };

    deletions.clear();
    return recurse();
}

}  // namespace

EulerianCycleResult EulerianCycleBuilder::compute(
    EulerizationMode mode,
    bool preferEulerization) const {
    EulerianCycleResult result;
    result.success = false;
    result.wasAlreadyEulerian = false;
    result.wasSemiEulerian = false;
    result.requiresMultigraph = false;
    result.traversalKind = EulerTraversalKind::None;

    result.modifiedGraph = cloneGraph(m_graph);
    AdjacencyGraph& G = *result.modifiedGraph;  // ? рабочая копия для эйлеризации

    if (G.isDirected()) {
        return result;  // ? лаба 5 — только неориентированный
    }

    if (G.edges().empty()) {
        return result;  // ? нет рёбер — обход не строим
    }

    // ? быстрый путь: уже связный — можно без add/remove рёбер
    const auto originalComponents = connectedComponentsWithEdges(G);
    if (originalComponents.size() <= 1) {
        std::vector<int> originalOddVertices;
        for (int v : G.vertexIds()) {
            if (degreeOf(G, v) % 2 == 1) {  // ? считаем нечётные степени
                originalOddVertices.push_back(v);
            }
        }
        std::sort(originalOddVertices.begin(), originalOddVertices.end());

        if (originalOddVertices.empty()) {
            // ? эйлеров граф: все степени чётные → цикл
            int start = -1;
            for (int v : G.vertexIds()) { // тут лишний гард. его можно убрать
                if (!G.neighbors(v).empty()) {
                    start = v;
                    break;
                }
            }
            if (start == -1) {
                return result;
            }

            result.traversal = hierholzer(G, start);  // ? старт любая вершина с ребром
            result.traversalKind = EulerTraversalKind::Cycle;
            result.wasAlreadyEulerian = true;
            result.success = true;
            return result;
        }

        // ? полуэйлеров: 2 нечётные, без эйлеризации — цепь, не цикл
        if (originalOddVertices.size() == 2 && !preferEulerization) {
            result.traversal = hierholzer(G, originalOddVertices.front());  // ? старт с нечётной
            result.traversalKind = EulerTraversalKind::Path;
            result.wasSemiEulerian = true;
            result.success = true;
            return result;
        }
        // ? иначе (preferEulerization или >2 нечётных) — ниже эйлеризация add/remove
    }

    // ? шаг 1 эйлеризации: склеить компоненты
    while (true) {
        auto components = connectedComponentsWithEdges(G);
        if (components.size() <= 1) break;

        const int u = components[0].front();
        const int v = components[1].front();
        G.addEdge(u, v, 1.0);  // ? ДОБАВЛЕНИЕ ребра
        result.additions.push_back({u, v, true, "connect components"});
    }

    // ? шаг 2: сделать все степени чётными (добавить или удалить рёбра)
    std::vector<int> oddVertices;
    for (int v : G.vertexIds()) {
        if (degreeOf(G, v) % 2 == 1) {
            oddVertices.push_back(v);
        }
    }

    std::sort(oddVertices.begin(), oddVertices.end());

    if (!oddVertices.empty()) {
        std::vector<std::pair<int,int>> pairs;
        bool foundClean = false;

        if (mode == EulerizationMode::NonMultigraphOnly) {
            const auto forbidden = existingEdgePairs(G);
            foundClean = findNonMultigraphMatching(oddVertices, forbidden, pairs);

            if (!foundClean) {
                result.requiresMultigraph = true;  // ? без дубля ребра не выйти
                result.success = false;
                return result;
            }
        } else if (mode == EulerizationMode::DeleteEdgesOnly) {  // ? чётность через удаление
            std::vector<std::pair<int,int>> deletions;
            const bool foundDeletionFix = findDeletionMatching(G, oddVertices, deletions);
            if (!foundDeletionFix) {
                result.requiresMultigraph = true;
                result.success = false;
                return result;
            }

            for (const auto& [u, v] : deletions) {
                result.additions.push_back({u, v, false, "fix odd parity by deleting edge"});  // ? журнал УДАЛЕНИЯ
            }
        } else {
            for (size_t i = 0; i + 1 < oddVertices.size(); i += 2) {
                pairs.push_back({oddVertices[i], oddVertices[i + 1]});  // ? AllowMultigraph: пары подряд
            }
        }

        for (const auto& [u, v] : pairs) {
            G.addEdge(u, v, 1.0);  // ? ДОБАВЛЕНИЕ ребра (чётность)
            result.additions.push_back({u, v, true, "fix odd parity"});
        }
    }

    result.wasAlreadyEulerian = result.additions.empty();

    // ? шаг 3: после эйлеризации все степени чётные → эйлеров ЦИКЛ
    int start = -1;
    for (int v : G.vertexIds()) {
        if (!G.neighbors(v).empty()) {
            start = v;
            break;
        }
    }
    if (start == -1) {
        return result;
    }

    result.traversal = hierholzer(G, start);  // ? обход по изменённому G (с добавл. рёбрами)
    result.traversalKind = EulerTraversalKind::Cycle;
    result.success = true;
    return result;
}
