#include "include/EdmondsMatching.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

/*
    Термины, которые используются ниже:

    Matching / паросочетание M:
        набор ребер, где никакие два ребра не имеют общей вершины.

    Matched / covered vertex:
        вершина, которая уже является концом какого-то ребра из M.

    Free / unmatched vertex:
        вершина, которая пока не покрыта ни одним ребром из M.

    Matched edge:
        ребро, которое уже входит в текущее паросочетание M.

    Unmatched edge:
        ребро графа, которое сейчас не входит в M.

    Alternating path / чередующийся путь:
        путь, где ребра идут по очереди:
        не из M, из M, не из M, из M, ...

    Augmenting path / аугментирующий путь:
        чередующийся путь, который начинается и заканчивается в свободных
        вершинах. Если его "перевернуть", matching станет больше на 1 ребро.

    Root / корневая вершина:
        свободная вершина, от которой мы начинаем BFS-поиск аугментирующего
        пути. В коде это параметр root в bfsAugment(root).

    Alternating tree / чередующееся дерево:
        BFS-дерево, которое строится от root. В нем путь от root до любой
        вершины тоже чередуется по ребрам: не из M, из M, не из M, ...

    Even-level vertex / вершина четного уровня:
        вершина, до которой от root идет путь четной длины. Именно такие
        вершины лежат в очереди BFS в этом коде.

    Odd-level vertex / вершина нечетного уровня:
        вершина, до которой от root идет путь нечетной длины. Если она покрыта
        matching, мы сразу "перепрыгиваем" через ее matched edge к парной
        вершине четного уровня.

    parent[v]:
        откуда мы пришли в вершину v в чередующемся дереве. По parent[] потом
        восстанавливается найденный аугментирующий путь.

    Blossom:
        нечетный цикл, найденный внутри чередующегося дерева. Его можно
        временно считать одной вершиной, чтобы BFS не застревал на цикле.

    Base / база blossom:
        вершина цикла, ближайшая к root. После сжатия blossom все вершины
        внутри него имеют одну общую базу в массиве base[].

    LCA:
        lowest common ancestor, то есть ближайший общий предок двух вершин
        в чередующемся дереве. В этом алгоритме LCA становится базой blossom.

    Contraction / shrink:
        временное сжатие blossom в одну "супервершину". В коде это делается
        не созданием нового графа, а перенаправлением base[].

    Алгоритм Эдмондса (blossom), общая идея:

    Нужно найти maximum matching, то есть самое большое по числу ребер
    паросочетание. Паросочетание M - это набор ребер, где никакие два ребра
    не имеют общей вершины.

    Алгоритм держит текущее M и ищет аугментирующий путь. Это путь, который
    начинается в свободной вершине, заканчивается в другой свободной вершине,
    а ребра в нем чередуются так:

        не из M, из M, не из M, из M, ..., не из M

    Если такой путь найден, ребра на нем "переворачиваются":
        те, что были в M, удаляются,
        те, которых не было в M, добавляются.
    После этого размер паросочетания увеличивается на 1.

    В двудольном графе для этого хватает обычного BFS по чередующимся путям.
    В общем графе мешают нечетные циклы. Такой нечетный цикл называется
    blossom. Эдмондс временно сжимает весь blossom в одну вершину, продолжает
    поиск, а потом по parent/base восстанавливает настоящий путь.

    Как это сделано именно тут:

    1. EdmondsWorker сначала переводит id вершин графа в индексы 0..n-1.
       Все массивы ниже работают с этими индексами. В самом конце индексы
       переводятся обратно в настоящие id вершин.

    2. m_match заполняется -1. Это значит, что начальное паросочетание пустое.
       Никакого жадного начального шага здесь нет.

    3. run() идет по всем вершинам. Если вершина v все еще свободна, вызывается
       findAugmentingPath(v). Это не значит, что v сразу добавляется в matching.
       Это значит только: пробуем построить от нее чередующееся BFS-дерево.

    4. В bfsAugment(root) начальный "путь" состоит только из root:
           queue = { root }
           parent[*] = -1
           base[i] = i
       Отдельным массивом путь во время поиска не хранится. Вместо этого
       запоминаются parent-ссылки. Сам путь потом восстанавливается назад
       от найденной свободной конечной вершины.

    5. В очереди BFS лежат вершины четного уровня чередующегося дерева.
       Для такой вершины u смотрим каждое ребро графа (u, v):

       - Если v свободна и еще не посещалась, то root ... u - v уже является
         аугментирующим путем. bfsAugment возвращает v.

       - Если v уже покрыта паросочетанием и ее пара - x, то продолжать путь
         можно только через ребро паросочетания v - x. Поэтому ставим
         parent[v] = u и кладем x = m_match[v] в очередь как новую вершину
         четного уровня.

       - Если v уже лежит в этом же чередующемся дереве так, что получается
         нечетный цикл, найден blossom. contractBlossom(u, v) находит его базу
         через findLCA(), отмечает обе стороны цикла и для всех вершин внутри
         цикла перенаправляет base[] на общую базу.

    6. Когда bfsAugment возвращает свободную конечную вершину, augment(endpoint)
       идет назад по parent[] и переворачивает ребра на найденном пути:

           endpoint <- parent[endpoint] <- matched partner <- ...

       После этого в matching становится на одно ребро больше.

    7. Если от root аугментирующий путь не найден, matching не меняется.
       Внешний цикл просто пробует следующую свободную вершину.

    Важно: этот алгоритм максимизирует количество ребер. Веса тут не участвуют
    в выборе, они нужны только для красивого вывода найденных ребер.
*/

namespace {

// Internal worker: the algorithm uses dense vertex indexes.
class EdmondsWorker {
public:
    EdmondsWorker(const AdjacencyGraph& graph)
        : m_graph(graph)
        , m_n(graph.size())
    {
        // Original id <-> dense index.
        m_vertexIds = graph.vertexIds();
        for (int i = 0; i < m_n; ++i) {
            m_idToIndex[m_vertexIds[i]] = i;
        }

        // Dense adjacency list. Self-loops are useless for matching.
        m_adj.assign(m_n, {});
        for (const WeightedEdge& edge : graph.edges()) {
            const int u = m_idToIndex[edge.from];
            const int v = m_idToIndex[edge.to];
            if (u == v) continue;  // skip self-loops
            m_adj[u].push_back(v);
            m_adj[v].push_back(u);
        }
    }

    /// Run Edmonds. match[i] == -1 means i is unmatched.
    std::vector<int> run() {
        m_match.assign(m_n, -1);

        // Try to grow the matching from every free vertex.
        for (int v = 0; v < m_n; ++v) {
            if (m_match[v] == -1) {
                findAugmentingPath(v);
            }
        }
        return m_match;
    }

private:
    // Graph data in dense indexes.
    const AdjacencyGraph& m_graph;
    int m_n;
    std::vector<int> m_vertexIds;          // dense index -> original ID
    std::unordered_map<int, int> m_idToIndex;  // original ID -> dense index
    std::vector<std::vector<int>> m_adj;   // dense adjacency

    // State for one BFS search.
    std::vector<int> m_match;     // match[v] = partner of v in M, or -1
    std::vector<int> m_parent;    // BFS-tree parent (in the alternating forest)
    std::vector<int> m_base;      // current blossom base for each vertex
    std::vector<int> m_q;         // BFS queue (dense indices)
    std::vector<bool> m_inQueue;  // BFS visited flag
    std::vector<bool> m_inBlossom; // marker used when contracting a blossom

    // Lowest common ancestor in the alternating forest.
    int findLCA(int a, int b) {
        std::vector<bool> visitedByA(m_n, false);

        // Mark the path from a to the root.
        int x = a;
        while (true) {
            x = m_base[x];
            visitedByA[x] = true;
            if (m_match[x] == -1) break;
            x = m_parent[m_match[x]];
        }

        // Climb from b until we hit that path.
        int y = b;
        while (true) {
            y = m_base[y];
            if (visitedByA[y]) return y;
            y = m_parent[m_match[y]];
        }
    }

    // Mark one side of a blossom.
    void markBlossomPath(int u, int b, int child) {
        while (m_base[u] != b) {
            m_parent[u] = child;
            child = m_match[u];
            if (!m_inQueue[child]) {
                m_q.push_back(child);
                m_inQueue[child] = true;
            }
            m_inBlossom[m_base[u]] = true;
            m_inBlossom[m_base[child]] = true;
            u = m_parent[child];
        }
    }

    // Shrink the blossom found through edge (u, v).
    void contractBlossom(int u, int v) {
        const int lca = findLCA(u, v);

        std::fill(m_inBlossom.begin(), m_inBlossom.end(), false);
        markBlossomPath(u, lca, v);
        markBlossomPath(v, lca, u);

        // Redirect all marked bases to the blossom base.
        for (int x = 0; x < m_n; ++x) {
            if (m_inBlossom[m_base[x]]) {
                m_base[x] = lca;
            }
        }
    }

    // BFS in the alternating forest. Returns free endpoint or -1.
    int bfsAugment(int root) {
        m_parent.assign(m_n, -1);
        m_inQueue.assign(m_n, false);
        m_inBlossom.assign(m_n, false);

        // Each vertex starts as its own base.
        m_base.resize(m_n);
        for (int i = 0; i < m_n; ++i) m_base[i] = i;

        m_q.clear();
        m_q.push_back(root);
        m_inQueue[root] = true;

        // Queue stores even-level vertices.
        for (size_t head = 0; head < m_q.size(); ++head) {
            const int u = m_q[head];
            for (int v : m_adj[u]) {
                // Skip the edge already used in matching.
                if (m_base[u] == m_base[v] || m_match[u] == v) continue;

                // Odd cycle in the tree: contract it.
                if (v == root || (m_match[v] != -1 && m_parent[m_match[v]] != -1)) {
                    contractBlossom(u, v);
                }
                // New vertex in the forest.
                else if (m_parent[v] == -1) {
                    m_parent[v] = u;
                    if (m_match[v] == -1) {
                        // Free vertex means augmenting path is found.
                        return v;
                    }
                    // Jump through the matched edge.
                    m_q.push_back(m_match[v]);
                    m_inQueue[m_match[v]] = true;
                }
            }
        }
        return -1;
    }

    // Flip edges along the found augmenting path.
    void augment(int v) {
        while (v != -1) {
            const int pv = m_parent[v];
            const int next = (pv == -1) ? -1 : m_match[pv];
            m_match[v] = pv;
            m_match[pv] = v;
            v = next;
        }
    }

    // One attempt to improve the matching.
    void findAugmentingPath(int root) {
        const int endpoint = bfsAugment(root);
        if (endpoint != -1) {
            augment(endpoint);
        }
    }
};

}

// Public wrapper: convert dense indexes back to graph vertex ids.
EdmondsMatching::EdmondsMatching(const AdjacencyGraph& graph)
    : m_graph(graph)
{}

MatchingResult EdmondsMatching::compute() const {
    MatchingResult result;
    result.matchingSize = 0;
    result.isPerfect = false;

    const int n = m_graph.size();
    if (n == 0) {
        return result;
    }

    EdmondsWorker worker(m_graph);
    const std::vector<int> match = worker.run();

    // Report each pair only once.
    const std::vector<int> vertexIds = m_graph.vertexIds();
    std::unordered_map<int, int> idToIndex;
    for (int i = 0; i < n; ++i) idToIndex[vertexIds[i]] = i;

    for (int i = 0; i < n; ++i) {
        const int j = match[i];
        if (j == -1 || j < i) continue;  // unmatched, or duplicate direction

        const int idA = vertexIds[i];
        const int idB = vertexIds[j];

        // Weight is only for output.
        auto weightOpt = m_graph.getEdgeWeight(idA, idB);
        if (!weightOpt.has_value()) {
            weightOpt = m_graph.getEdgeWeight(idB, idA);
        }
        const double w = weightOpt.value_or(0.0);

        result.matchingEdges.push_back({idA, idB, w});
    }

    // Stable output order.
    std::sort(result.matchingEdges.begin(), result.matchingEdges.end(),
        [](const WeightedEdge& a, const WeightedEdge& b) {
            if (a.from != b.from) return a.from < b.from;
            return a.to < b.to;
        });

    result.matchingSize = static_cast<int>(result.matchingEdges.size());

    // Uncovered vertices.
    for (int i = 0; i < n; ++i) {
        if (match[i] == -1) {
            result.unmatchedVertices.push_back(vertexIds[i]);
        }
    }
    result.isPerfect = result.unmatchedVertices.empty();

    return result;
}
