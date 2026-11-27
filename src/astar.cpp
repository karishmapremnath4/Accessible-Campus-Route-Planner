#include "route.h"
#include "min_heap.h"
#include <limits>

// A* search. Follows docs/PSEUDOCODE_ASTAR.md.
//
// This is Dijkstra with one change: the priority queue is ordered by the cost
// so far plus an estimate of the cost still to come, instead of by the cost so
// far alone. The estimate pushes the search towards the destination rather
// than letting it spread evenly in every direction.
//
// The cost array still holds the real accumulated cost. The estimate only
// affects the order buildings come off the queue, never what is stored.
RouteResult aStar(const Graph& g, int start, int destination, Preference pref) {
    const double INF = std::numeric_limits<double>::infinity();
    int n = g.buildings.size;

    RouteResult result;
    if (start < 0 || start >= n || destination < 0 || destination >= n) {
        return result;
    }

    DynamicArray<double> cost;      // real cost from the start
    DynamicArray<int> prev;         // previous building on the best route
    DynamicArray<Edge> prevEdge;    // pathway used to reach each building
    DynamicArray<bool> done;        // finalized flags
    for (int i = 0; i < n; i++) {
        cost.push_back(INF);
        prev.push_back(-1);
        Edge none{};
        prevEdge.push_back(none);
        done.push_back(false);
    }

    cost[start] = 0.0;
    MinHeap heap;
    heap.push(start, heuristic(g, start, destination));

    while (!heap.empty()) {
        HeapItem top = heap.pop();
        int u = top.building;
        if (done[u]) continue;          // stale entry left in the heap
        done[u] = true;
        result.buildingsExplored++;
        if (u == destination) break;

        for (int k = 0; k < g.adj[u].size; k++) {
            const Edge& e = g.adj[u][k];
            if (!isPathAccessible(e, pref)) continue;
            double candidate = cost[u] + pathCost(e, pref);
            if (candidate < cost[e.to]) {
                cost[e.to] = candidate;
                prev[e.to] = u;
                prevEdge[e.to] = e;
                // The estimate is added only for the queue ordering.
                heap.push(e.to, candidate + heuristic(g, e.to, destination));
            }
        }
    }

    if (!done[destination]) {
        return result;                  // found stays false
    }

    // Walk back from the destination, collecting buildings and pathways.
    DynamicArray<int> backBuildings;
    DynamicArray<Edge> backPathways;
    int at = destination;
    while (at != start) {
        backBuildings.push_back(at);
        backPathways.push_back(prevEdge[at]);
        at = prev[at];
    }
    backBuildings.push_back(start);

    // Reverse so the route runs from start to destination.
    for (int i = backBuildings.size - 1; i >= 0; i--) {
        result.buildings.push_back(backBuildings[i]);
    }
    for (int i = backPathways.size - 1; i >= 0; i--) {
        result.pathways.push_back(backPathways[i]);
        result.distance += backPathways[i].distance;   // real distance
    }
    result.found = true;
    return result;
}
