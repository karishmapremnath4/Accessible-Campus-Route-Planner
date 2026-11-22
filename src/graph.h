#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include "dynamic_array.h"

// One campus building. The id is also the building's position in the
// buildings array, so buildings[id] is that building.
struct Building {
    int id;
    std::string code;   // the code Northeastern uses, e.g. SL for Snell Library
    std::string name;
    double x;           // meters east of Snell Library
    double y;           // meters north of Snell Library
};

// One pathway leading out of a building. Each row of pathways.csv becomes two
// of these, one in each direction, because walking works both ways.
struct Edge {
    int to;
    double distance;    // meters
    bool hasStairs;
    bool isClosed;
    bool isIndoor;      // true if the route runs through the campus tunnels
};

// What the user asked for.
//   STANDARD          the shortest route, whatever it uses
//   NO_STAIRS         a route that never uses steps
//   INDOOR_PREFERRED  a route that favours the tunnels, used in bad weather
enum Preference {
    STANDARD = 1,
    NO_STAIRS = 2,
    INDOOR_PREFERRED = 3
};

// Weight applied to an outdoor pathway under INDOOR_PREFERRED. A tunnel is
// usually longer than the walk above it, so without this the search would
// never choose one. 1.5 makes a tunnel win when it is less than half again
// as long as the surface route.
const double OUTDOOR_WEIGHT = 1.5;

// The campus graph. adj[i] holds every pathway leading out of building i.
struct Graph {
    DynamicArray<Building> buildings;
    DynamicArray< DynamicArray<Edge> > adj;
};

// Reads both CSV files and fills g. Returns false and prints a message if a
// file cannot be opened or a line is not valid.
bool loadCampusData(const std::string& buildingsFile,
                    const std::string& pathwaysFile,
                    Graph& g);

// True if the search is allowed to use this pathway under the preference.
bool isPathAccessible(const Edge& e, Preference pref);

// What this pathway costs the search. Always its real distance, except under
// INDOOR_PREFERRED where an outdoor pathway is weighted so tunnels win.
double pathCost(const Edge& e, Preference pref);

// Straight-line distance between two buildings, in meters. Used by A* to
// estimate how far is left to go.
double heuristic(const Graph& g, int from, int to);

#endif
