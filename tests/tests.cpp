// Tests for the campus data, the graph loader, MinHeap and DynamicArray.
//
// Build and run from the top of the repository, because the tests read the
// files in data/ and write short-lived files in tests/:
//
//   g++ -std=c++17 -Wall -Wextra -I src -o run_tests tests/tests.cpp src/graph.cpp
//   ./run_tests
//
// Each failed check prints one line. The last line is a summary, and the
// program exits with 1 if anything failed. Pass a word to run only the tests
// whose name contains it, for example: ./run_tests heap

#include "graph.h"
#include "min_heap.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static const char* currentTest = "";
static int checks = 0;
static int failures = 0;

static void check(bool ok, const std::string& what) {
    checks++;
    if (!ok) {
        failures++;
        std::cout << "FAIL [" << currentTest << "] " << what << "\n";
    }
}

static int countEntries(const Graph& g) {
    int n = 0;
    for (int i = 0; i < g.adj.size; i++) {
        n += g.adj[i].size;
    }
    return n;
}

// Calls loadCampusData and keeps what it prints off the screen, so the
// expected error messages do not clutter the test output.
static bool load(const std::string& buildingsFile, const std::string& pathwaysFile,
                 Graph& g, std::string& printed) {
    std::ostringstream captured;
    std::streambuf* screen = std::cout.rdbuf(captured.rdbuf());
    bool ok = loadCampusData(buildingsFile, pathwaysFile, g);
    std::cout.rdbuf(screen);
    printed = captured.str();
    return ok;
}

static void writeFile(const char* path, const std::string& text) {
    std::ofstream out(path, std::ios::binary);   // binary keeps \r\n exactly as written
    out << text;
}

// Same as load, but the two files are given as text. They are written to
// tests/, loaded, and deleted again.
static bool loadText(const std::string& buildings, const std::string& pathways,
                     Graph& g, std::string& printed) {
    const char* bfile = "tests/tmp_buildings.csv";
    const char* pfile = "tests/tmp_pathways.csv";
    writeFile(bfile, buildings);
    writeFile(pfile, pathways);
    bool ok = load(bfile, pfile, g, printed);
    std::remove(bfile);
    std::remove(pfile);
    return ok;
}

static std::string withWindowsLineEndings(const std::string& text) {
    std::string out;
    for (char c : text) {
        if (c == '\n') {
            out += '\r';
        }
        out += c;
    }
    return out;
}

// A tiny campus: two buildings 50 m apart in a straight line (a 30-40-50
// triangle), joined by one 60 m pathway.
static const std::string BUILDINGS_HEADER = "id,code,name,lat,lon,x,y\n";
static const std::string PATHWAYS_HEADER = "from,to,distance,has_stairs,is_closed,is_indoor,source\n";
static const std::string TWO_BUILDINGS = BUILDINGS_HEADER + "0,A,Alpha,0,0,0,0\n" + "1,B,Beta,0,0,30,40\n";
static const std::string ONE_PATHWAY = PATHWAYS_HEADER + "0,1,60,0,0,0,test\n";

// ---- Loader ---------------------------------------------------------------

static void testShippedData() {
    Graph g;
    std::string printed;
    bool ok = load("data/buildings.csv", "data/pathways.csv", g, printed);
    check(ok, "loadCampusData returns true");
    check(g.buildings.size == 39, "39 buildings");
    check(countEntries(g) == 302, "302 directed entries, two per pathway");
    check(printed.empty(), "no messages printed");
}

static void testLoadingAgainReplacesGraph() {
    Graph g;
    std::string printed;
    load("data/buildings.csv", "data/pathways.csv", g, printed);
    bool ok = loadText(TWO_BUILDINGS, ONE_PATHWAY, g, printed);
    check(ok, "a second load into the same Graph succeeds");
    check(g.buildings.size == 2, "the second load replaces the buildings");
    check(countEntries(g) == 2, "the second load replaces the pathways");
}

static void testFailedLoadChangesNothing() {
    Graph g;
    std::string printed;
    bool ok = loadText(TWO_BUILDINGS, ONE_PATHWAY + "0,1,abc,0,0,0,test\n", g, printed);
    check(!ok, "a bad row makes the load fail");
    check(g.buildings.size == 0 && g.adj.size == 0, "the Graph is left empty, not half-filled");
}

static void testWindowsLineEndings() {
    Graph g;
    std::string printed;
    bool ok = loadText(withWindowsLineEndings(TWO_BUILDINGS),
                       withWindowsLineEndings(ONE_PATHWAY), g, printed);
    check(ok, "files with Windows line endings load");
    check(g.buildings.size == 2 && g.buildings[1].y == 40.0, "the last column is read correctly");
    check(countEntries(g) == 2, "the pathway is read");
}

static void testBlankLinesWithSpaces() {
    Graph g;
    std::string printed;
    bool ok = loadText(BUILDINGS_HEADER + "0,A,Alpha,0,0,0,0\n   \n1,B,Beta,0,0,30,40\n",
                       ONE_PATHWAY + "\t\n", g, printed);
    check(ok, "lines holding only spaces or tabs are skipped");
    check(g.buildings.size == 2 && countEntries(g) == 2, "every real row is still read");
}

static void testMissingHeader() {
    Graph g;
    std::string printed;
    bool ok = loadText(TWO_BUILDINGS, "0,1,60,0,0,0,test\n", g, printed);
    check(!ok, "a pathways file without its header row is rejected");
    check(printed.find("header") != std::string::npos, "the message mentions the header");
}

static void testByteOrderMark() {
    Graph g;
    std::string printed;
    bool ok = loadText("\xEF\xBB\xBF" + TWO_BUILDINGS, ONE_PATHWAY, g, printed);
    check(ok, "a file saved as UTF-8 with a byte order mark, as Excel does, loads");
}

static void testHeaderOnlyPathways() {
    Graph g;
    std::string printed;
    bool ok = loadText(TWO_BUILDINGS, PATHWAYS_HEADER, g, printed);
    check(!ok, "a pathways file with no pathways is rejected");
    check(printed.find("no pathways") != std::string::npos, "the message says there are no pathways");
}

static void testPathwayShorterThanStraightLine() {
    Graph g;
    std::string printed;
    bool ok = loadText(TWO_BUILDINGS, PATHWAYS_HEADER + "0,1,45,0,0,0,test\n", g, printed);
    check(!ok, "a 45 m pathway between buildings 50 m apart is rejected");
    check(printed.find("straight") != std::string::npos, "the message mentions the straight-line rule");
}

static void testHugeNumber() {
    Graph g;
    std::string printed;
    bool ok = loadText(BUILDINGS_HEADER + "99999999999,A,Alpha,0,0,0,0\n", ONE_PATHWAY, g, printed);
    check(!ok, "an id too big for an int is rejected");
    check(printed.find("not a number") != std::string::npos, "the message says it is not a number");
}

// ---- MinHeap --------------------------------------------------------------

static void testHeapOrder() {
    MinHeap h;
    double priorities[] = {5, 3, 8, 3, 1, 9, 2, 7, 3};
    for (int i = 0; i < 9; i++) {
        h.push(i, priorities[i]);
    }
    double last = -1;
    int popped = 0;
    bool inOrder = true;
    while (!h.empty()) {
        HeapItem item = h.pop();
        if (item.priority < last) {
            inOrder = false;
        }
        last = item.priority;
        popped++;
    }
    check(popped == 9, "every pushed item comes back out");
    check(inOrder, "items come out smallest first, duplicates included");
}

static void testHeapPopWhenEmpty() {
    MinHeap h;
    h.push(1, 5.0);
    h.pop();
    HeapItem none = h.pop();   // nothing left to pop
    check(none.building == -1, "popping an empty heap returns building -1");
    check(h.empty(), "the heap is still empty afterwards");
    if (h.empty()) {           // only safe to carry on if the heap was not corrupted
        h.push(7, 1.0);
        check(!h.empty() && h.pop().building == 7, "the heap still works after an empty pop");
    }
}

// ---- DynamicArray ---------------------------------------------------------

static void testPushOwnElement() {
    DynamicArray<std::string> a;
    std::string first(40, 'a');   // too long for the small-string buffer, so it lives on the heap
    a.push_back(first);
    a.push_back(std::string(40, 'b'));
    a.push_back(std::string(40, 'c'));
    a.push_back(std::string(40, 'd'));   // the array is now full (capacity 4)
    a.push_back(a[0]);                   // has to grow while a[0] is being copied
    check(a.size == 5 && a[4] == first, "push_back(a[0]) on a full array copies the right value");
}

// ---- Campus data and the test files ----------------------------------------

static const int SNELL_LIBRARY = 0;
static const int EXP_BUILDING = 9;

// Marks every building that can be reached from start using only the
// pathways the preference allows.
static void findReachable(const Graph& g, int start, Preference pref, DynamicArray<bool>& reached) {
    for (int i = 0; i < g.buildings.size; i++) {
        reached.push_back(false);
    }
    DynamicArray<int> toVisit;
    toVisit.push_back(start);
    reached[start] = true;
    while (toVisit.size > 0) {
        int u = toVisit[toVisit.size - 1];
        toVisit.size--;
        for (int k = 0; k < g.adj[u].size; k++) {
            const Edge& e = g.adj[u][k];
            if (isPathAccessible(e, pref) && !reached[e.to]) {
                reached[e.to] = true;
                toVisit.push_back(e.to);
            }
        }
    }
}

static int countTrue(const DynamicArray<bool>& flags) {
    int n = 0;
    for (int i = 0; i < flags.size; i++) {
        if (flags[i]) {
            n++;
        }
    }
    return n;
}

static void testNoRouteFile() {
    Graph g;
    std::string printed;
    bool ok = load("data/buildings.csv", "data/test_no_route_pathways.csv", g, printed);
    check(ok, "test_no_route_pathways.csv loads");
    if (!ok) {
        return;
    }
    Preference prefs[] = {STANDARD, NO_STAIRS, INDOOR_PREFERRED};
    for (Preference pref : prefs) {
        DynamicArray<bool> reached;
        findReachable(g, SNELL_LIBRARY, pref, reached);
        check(!reached[EXP_BUILDING] && countTrue(reached) == 38,
              "only EXP is cut off under preference " + std::to_string(pref));
    }
}

static void testStairsOnlyFile() {
    Graph g;
    std::string printed;
    bool ok = load("data/buildings.csv", "data/test_stairs_only_pathways.csv", g, printed);
    check(ok, "test_stairs_only_pathways.csv loads");
    if (!ok) {
        return;
    }
    DynamicArray<bool> standard;
    DynamicArray<bool> noStairs;
    DynamicArray<bool> indoor;
    findReachable(g, SNELL_LIBRARY, STANDARD, standard);
    findReachable(g, SNELL_LIBRARY, NO_STAIRS, noStairs);
    findReachable(g, SNELL_LIBRARY, INDOOR_PREFERRED, indoor);
    check(countTrue(standard) == 39 && countTrue(indoor) == 39,
          "with stairs allowed every building can be reached");
    check(!noStairs[EXP_BUILDING] && countTrue(noStairs) == 38,
          "avoiding stairs cuts off EXP and nothing else");
}

// ---- Runner ----------------------------------------------------------------

struct Test {
    const char* name;
    void (*run)();
};

static const Test TESTS[] = {
    {"loader: shipped data", testShippedData},
    {"loader: loading again replaces the graph", testLoadingAgainReplacesGraph},
    {"loader: a failed load changes nothing", testFailedLoadChangesNothing},
    {"loader: Windows line endings", testWindowsLineEndings},
    {"loader: blank lines with spaces", testBlankLinesWithSpaces},
    {"loader: missing header", testMissingHeader},
    {"loader: byte order mark", testByteOrderMark},
    {"loader: header-only pathways file", testHeaderOnlyPathways},
    {"loader: pathway shorter than the straight line", testPathwayShorterThanStraightLine},
    {"loader: huge number", testHugeNumber},
    {"heap: pops in order", testHeapOrder},
    {"heap: pop when empty", testHeapPopWhenEmpty},
    {"array: push its own element", testPushOwnElement},
    {"data: no-route test file", testNoRouteFile},
    {"data: stairs-only test file", testStairsOnlyFile},
};

int main(int argc, char** argv) {
    std::string only = argc > 1 ? argv[1] : "";
    int ran = 0;
    for (const Test& t : TESTS) {
        if (std::string(t.name).find(only) == std::string::npos) {
            continue;
        }
        currentTest = t.name;
        t.run();
        ran++;
    }
    std::cout << ran << " tests, " << checks << " checks, " << failures << " failed\n";
    return failures == 0 ? 0 : 1;
}
