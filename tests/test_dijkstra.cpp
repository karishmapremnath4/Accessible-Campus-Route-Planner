#include "graph.h"
#include "route.h"
#include <iostream>
#include <string>

static int checks = 0;
static int failures = 0;

static void check(bool ok, const std::string& what) {
    checks++;
    if (!ok) {
        failures++;
        std::cout << "FAIL " << what << "\n";
    }
}

static const int SNELL_LIBRARY = 0;
static const int CURRY_STUDENT_CENTER = 1;
static const int EXP = 9;

static void testSnellToCurryStandard() {
    Graph g;
    check(loadCampusData("data/buildings.csv", "data/pathways.csv", g), "shipped data loads");
    RouteResult r = dijkstra(g, SNELL_LIBRARY, CURRY_STUDENT_CENTER, STANDARD);
    check(r.found, "Snell to Curry is found");
    check(r.distance == 86.0, "Snell to Curry takes the 86 m tunnel, not the closed 72 m walk");
    check(r.buildings.size == 2 && r.buildings[0] == SNELL_LIBRARY
          && r.buildings[1] == CURRY_STUDENT_CENTER, "route is Snell then Curry");
    check(r.pathways.size == 1, "one pathway is used");
}

static void testSameStartAndDestination() {
    Graph g;
    loadCampusData("data/buildings.csv", "data/pathways.csv", g);
    RouteResult r = dijkstra(g, SNELL_LIBRARY, SNELL_LIBRARY, STANDARD);
    check(r.found, "start equal to destination is found");
    check(r.buildings.size == 1, "route holds one building");
    check(r.distance == 0.0, "distance is 0");
}

static void testNoRouteFile() {
    Graph g;
    loadCampusData("data/buildings.csv", "data/test_no_route_pathways.csv", g);
    RouteResult r = dijkstra(g, SNELL_LIBRARY, EXP, STANDARD);
    check(!r.found, "EXP is not found when both its pathways are closed");
}

static void testStairsOnlyFile() {
    Graph g;
    loadCampusData("data/buildings.csv", "data/test_stairs_only_pathways.csv", g);
    RouteResult standard = dijkstra(g, SNELL_LIBRARY, EXP, STANDARD);
    RouteResult noStairs = dijkstra(g, SNELL_LIBRARY, EXP, NO_STAIRS);
    check(standard.found, "EXP is found under STANDARD");
    check(!noStairs.found, "EXP is not found under NO_STAIRS");
}

static void testNoStairsRouteHasNoStairs() {
    Graph g;
    loadCampusData("data/buildings.csv", "data/pathways.csv", g);
    RouteResult r = dijkstra(g, SNELL_LIBRARY, CURRY_STUDENT_CENTER, NO_STAIRS);
    check(r.found, "Snell to Curry is found under NO_STAIRS");
    bool anyStairs = false;
    for (int i = 0; i < r.pathways.size; i++) {
        if (r.pathways[i].hasStairs) anyStairs = true;
    }
    check(!anyStairs, "no pathway on the NO_STAIRS route has stairs");
}

static void testOutOfRangeBuilding() {
    Graph g;
    loadCampusData("data/buildings.csv", "data/pathways.csv", g);
    RouteResult r = dijkstra(g, -1, SNELL_LIBRARY, STANDARD);
    check(!r.found, "a start building below 0 is rejected");
    r = dijkstra(g, SNELL_LIBRARY, 39, STANDARD);
    check(!r.found, "a destination past the last building is rejected");
}

int main() {
    testSnellToCurryStandard();
    testSameStartAndDestination();
    testNoRouteFile();
    testStairsOnlyFile();
    testNoStairsRouteHasNoStairs();
    testOutOfRangeBuilding();
    std::cout << checks << " checks, " << failures << " failed\n";
    return failures == 0 ? 0 : 1;
}