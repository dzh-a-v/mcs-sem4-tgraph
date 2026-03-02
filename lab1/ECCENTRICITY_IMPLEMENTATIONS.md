# Eccentricity Calculation - Two Implementations

## Overview

This project contains **two implementations** of eccentricity calculation:

1. **Shimbell's Method** (MAIN - used in program)
2. **Bellman-Ford** (REFERENCE - saved for comparison)

---

## 1. Shimbell's Method Implementation ✅

**Files:**
- `include/Eccentricity.h`
- `src/Eccentricity.cpp`

**Algorithm:**
```
For each vertex v:
  1. Run Shimbell for k = 1, 2, ..., V-1
  2. For each pair (v, u), take minimum distance across all k
  3. Eccentricity e(v) = max distance to any reachable vertex
```

**Why Shimbell?**
- Lab requirement: "Реализовать метод Шимбелла"
- Only Shimbell's method allowed for all calculations
- No Bellman-Ford in main program

**Complexity:** O(V⁵)
- Runs Shimbell V-1 times
- Each Shimbell call: O(V³)
- Total: O(V⁴) for all pairs, all k values

**Code Location:**
```cpp
// In Eccentricity.cpp
std::map<std::pair<int,int>, double> GraphAnalyzer::computeAllShortestPaths() const {
    // Run Shimbell for k = 1, 2, ..., V-1
    for (int k = 1; k < m_vertexCount; ++k) {
        KPathCalculator calculator(m_graph);
        auto result = calculator.compute(k);
        // Update minimum distances...
    }
}
```

---

## 2. Bellman-Ford Implementation (Reference)

**Files:**
- `include/EccentricityBF.h`
- `src/EccentricityBF.cpp`

**Algorithm:**
```
For each vertex v:
  1. Run Bellman-Ford from v
  2. Get shortest paths to all other vertices
  3. Eccentricity e(v) = max distance to any reachable vertex
```

**Why Saved?**
- Reference implementation
- Faster: O(V²E) vs O(V⁵)
- Supports negative weights naturally
- Can be used for comparison/verification

**NOT used in main program** - only for reference.

**Code Location:**
```cpp
// In EccentricityBF.cpp
std::map<int, double> GraphAnalyzerBF::runBellmanFord(int source) const {
    // Standard Bellman-Ford relaxation
    for (int iteration = 0; iteration < vertexCount - 1; ++iteration) {
        for each edge (u, v):
            relax(u, v)
    }
}
```

---

## File Structure

```
lab1/
├── include/
│   ├── Eccentricity.h       ← Shimbell version (ACTIVE)
│   └── EccentricityBF.h     ← Bellman-Ford version (REFERENCE)
│
├── src/
│   ├── Eccentricity.cpp     ← Shimbell version (ACTIVE)
│   └── EccentricityBF.cpp   ← Bellman-Ford version (REFERENCE)
│
└── CMakeLists.txt           ← Includes both versions
```

---

## How to Use

### Main Program (Uses Shimbell)

```cpp
#include "include/Eccentricity.h"  // Shimbell version

GraphAnalyzer analyzer(*graph);
auto result = analyzer.analyze();

std::cout << "Radius: " << result.radius << "\n";
std::cout << "Center: ";
for (int v : result.centerVertices) std::cout << "v" << v << " ";
```

### Reference Implementation (Bellman-Ford)

```cpp
#include "include/EccentricityBF.h"  // Bellman-Ford version

GraphAnalyzerBF analyzerBF(*graph);
auto resultBF = analyzerBF.analyze();

// Compare with Shimbell result
// (for verification/testing only)
```

---

## Comparison

| Aspect | Shimbell Version | Bellman-Ford Version |
|--------|-----------------|---------------------|
| **Files** | `Eccentricity.h/cpp` | `EccentricityBF.h/cpp` |
| **Class Name** | `GraphAnalyzer` | `GraphAnalyzerBF` |
| **Algorithm** | Shimbell (k=1 to V-1) | Bellman-Ford |
| **Complexity** | O(V⁵) | O(V² × E) |
| **Used in Main** | ✅ YES | ❌ NO |
| **Purpose** | Lab requirement | Reference only |
| **Negative Weights** | ✅ Supported | ✅ Supported |

---

## Why Two Versions?

1. **Lab Requirement:** Teacher said "only Shimbell's method"
2. **Comparison:** Can verify Shimbell results against Bellman-Ford
3. **Learning:** Shows two different approaches to same problem
4. **Flexibility:** Easy to switch if needed

---

## Verification (Optional)

To verify both implementations give same results:

```cpp
// In main.cpp (for testing)
GraphAnalyzer shimbell(*graph);
auto resultS = shimbell.analyze();

GraphAnalyzerBF bellmanFord(*graph);
auto resultBF = bellmanFord.analyze();

// Compare results
std::cout << "Shimbell radius: " << resultS.radius << "\n";
std::cout << "Bellman-Ford radius: " << resultBF.radius << "\n";
// Should be equal (within floating-point precision)
```

---

## Notes

- Both versions support **negative weights**
- Both versions handle **disconnected graphs** (unreachable vertices)
- Shimbell version is **slower** but meets lab requirements
- Bellman-Ford version is **faster** but not allowed for main program

---

## Build

Both files are compiled:
```cmake
add_executable(lab1
    ...
    src/Eccentricity.cpp      # Shimbell
    src/EccentricityBF.cpp    # Bellman-Ford
)
```

But only `GraphAnalyzer` (Shimbell) is used in `main.cpp`.
