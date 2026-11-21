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

#endif
