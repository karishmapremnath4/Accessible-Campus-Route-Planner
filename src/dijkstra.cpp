#include "route.h"
#include "min_heap.h"
#include <limits>

RouteResult dijkstra(const Graph& g, int start, int destination, Preference pref) {
    const double INF = std::numeric_limits<double>::infinity();
    int n = g.buildings.size;

    RouteResult result;
    if (start < 0 || start >= n || destination < 0 || destination >= n) {
        return result;
    }

    DynamicArray<double> cost;
    DynamicArray<int> prev;
    DynamicArray<Edge> prevEdge;
    DynamicArray<bool> done;
    for (int i = 0; i < n; i++) {
        cost.push_back(INF);
        prev.push_back(-1);
        Edge none;
        prevEdge.push_back(none);
        done.push_back(false);
    }

    cost[start] = 0.0;
    MinHeap heap;
    heap.push(start, 0.0);

    while (!heap.empty()) {
        HeapItem top = heap.pop();
        int u = top.building;
        if (done[u]) continue;
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
                heap.push(e.to, candidate);
            }
        }
    }

    if (!done[destination]) {
        return result;
    }

    DynamicArray<int> backBuildings;
    DynamicArray<Edge> backPathways;
    int at = destination;
    while (at != start) {
        backBuildings.push_back(at);
        backPathways.push_back(prevEdge[at]);
        at = prev[at];
    }
    backBuildings.push_back(start);

    for (int i = backBuildings.size - 1; i >= 0; i--) {
        result.buildings.push_back(backBuildings[i]);
    }
    for (int i = backPathways.size - 1; i >= 0; i--) {
        result.pathways.push_back(backPathways[i]);
        result.distance += backPathways[i].distance;
    }
    result.found = true;
    return result;
}