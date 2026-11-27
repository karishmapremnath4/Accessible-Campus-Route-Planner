#include "graph.h"
#include "route.h"
#include <iostream>
#include <cmath>
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
    RouteResult r = aStar(g, SNELL_LIBRARY, CURRY_STUDENT_CENTER, STANDARD);
    check(r.found, "Snell to Curry is found");
    check(r.distance == 86.0, "Snell to Curry takes the 86 m tunnel, not the closed 72 m walk");
    check(r.buildings.size == 2, "route holds two buildings");
}

static void testSameStartAndDestination() {
    Graph g;
    loadCampusData("data/buildings.csv", "data/pathways.csv", g);
    RouteResult r = aStar(g, SNELL_LIBRARY, SNELL_LIBRARY, STANDARD);
    check(r.found, "start equal to destination is found");
    check(r.buildings.size == 1, "route holds one building");
    check(r.distance == 0.0, "distance is 0");
}

static void testNoRouteFile() {
    Graph g;
    loadCampusData("data/buildings.csv", "data/test_no_route_pathways.csv", g);
    RouteResult r = aStar(g, SNELL_LIBRARY, EXP, STANDARD);
    check(!r.found, "EXP is unreachable when both of its pathways are closed");
}

static void testNoStairsRouteHasNoStairs() {
    Graph g;
    loadCampusData("data/buildings.csv", "data/pathways.csv", g);
    RouteResult r = aStar(g, SNELL_LIBRARY, EXP, NO_STAIRS);
    check(r.found, "Snell to EXP is found under NO_STAIRS");
    bool anyStairs = false;
    for (int i = 0; i < r.pathways.size; i++) {
        if (r.pathways[i].hasStairs) anyStairs = true;
    }
    check(!anyStairs, "no pathway on the NO_STAIRS route has stairs");
}

static void testOutOfRangeBuilding() {
    Graph g;
    loadCampusData("data/buildings.csv", "data/pathways.csv", g);
    RouteResult r = aStar(g, -1, SNELL_LIBRARY, STANDARD);
    check(!r.found, "a start building below 0 is rejected");
    r = aStar(g, SNELL_LIBRARY, g.buildings.size, STANDARD);
    check(!r.found, "a destination past the last building is rejected");
}

// The important one. If the heuristic ever overestimates, A* can return a
// route that is not the shortest, and the only way to see it is to compare
// against Dijkstra on every pair.
static void testAStarMatchesDijkstraEverywhere() {
    const char* sets[3][3] = {
        {"data/campus_15_buildings.csv", "data/campus_15_pathways.csv", "campus_15"},
        {"data/campus_25_buildings.csv", "data/campus_25_pathways.csv", "campus_25"},
        {"data/buildings.csv",           "data/pathways.csv",           "campus_39"}
    };
    Preference prefs[3] = {STANDARD, NO_STAIRS, INDOOR_PREFERRED};

    int compared = 0;
    bool allMatch = true;
    bool aStarNeverExploresMore = true;

    for (int s = 0; s < 3; s++) {
        Graph g;
        if (!loadCampusData(sets[s][0], sets[s][1], g)) {
            check(false, std::string("dataset loads: ") + sets[s][2]);
            continue;
        }
        for (int p = 0; p < 3; p++) {
            for (int a = 0; a < g.buildings.size; a++) {
                for (int b = 0; b < g.buildings.size; b++) {
                    if (a == b) continue;
                    RouteResult d = dijkstra(g, a, b, prefs[p]);
                    RouteResult x = aStar(g, a, b, prefs[p]);
                    compared++;
                    if (d.found != x.found) allMatch = false;
                    if (d.found && std::fabs(d.distance - x.distance) > 1e-9) allMatch = false;
                    if (x.buildingsExplored > d.buildingsExplored) aStarNeverExploresMore = false;
                }
            }
        }
    }
    check(compared == 6876, "every pair in all three datasets under all three preferences was compared");
    check(allMatch, "A* returns the same distance as Dijkstra on every pair");
    check(aStarNeverExploresMore, "A* never explores more buildings than Dijkstra");
}

int main() {
    testSnellToCurryStandard();
    testSameStartAndDestination();
    testNoRouteFile();
    testNoStairsRouteHasNoStairs();
    testOutOfRangeBuilding();
    testAStarMatchesDijkstraEverywhere();
    std::cout << checks << " checks, " << failures << " failed\n";
    return failures == 0 ? 0 : 1;
}
