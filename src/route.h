#ifndef ROUTE_H
#define ROUTE_H

#include "graph.h"

// The result of a route search. Dijkstra and A* both return this.
struct RouteResult {
    bool found = false;            // false if there is no route
    DynamicArray<int> buildings;   // start first, destination last
    DynamicArray<Edge> pathways;   // the pathway used for each step
    double distance = 0.0;         // real meters, not the weighted cost
    int buildingsExplored = 0;     // buildings taken off the queue
};

RouteResult dijkstra(const Graph& g, int start, int destination, Preference pref);
RouteResult aStar(const Graph& g, int start, int destination, Preference pref);

#endif
