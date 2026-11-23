#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

// Turns text into an int. Returns false if the text is not a whole number,
// so that a bad file produces a clear message instead of an exception.
static bool toInt(const std::string& s, int& out) {
    if (s.empty()) return false;
    int i = 0;
    bool negative = false;
    if (s[0] == '-' || s[0] == '+') {
        negative = (s[0] == '-');
        i = 1;
        if (s.size() == 1) return false;
    }
    // long long, not long: on Windows long is only 32 bits, so value * 10
    // could overflow before the check below catches a number that is too big.
    long long value = 0;
    for (; i < (int)s.size(); i++) {
        if (s[i] < '0' || s[i] > '9') return false;
        value = value * 10 + (s[i] - '0');
        if (value > 2147483647LL) return false;
    }
    out = (int)(negative ? -value : value);
    return true;
}

// Turns text into a double. Accepts an optional sign, digits, and an
// optional decimal part. Returns false on anything else.
static bool toDouble(const std::string& s, double& out) {
    if (s.empty()) return false;
    int i = 0;
    bool negative = false;
    if (s[0] == '-' || s[0] == '+') {
        negative = (s[0] == '-');
        i = 1;
        if (s.size() == 1) return false;
    }
    double value = 0.0;
    int digits = 0;
    for (; i < (int)s.size() && s[i] != '.'; i++) {
        if (s[i] < '0' || s[i] > '9') return false;
        value = value * 10.0 + (s[i] - '0');
        digits++;
    }
    if (i < (int)s.size() && s[i] == '.') {
        i++;
        double place = 0.1;
        for (; i < (int)s.size(); i++) {
            if (s[i] < '0' || s[i] > '9') return false;
            value += (s[i] - '0') * place;
            place /= 10.0;
            digits++;
        }
    }
    if (digits == 0) return false;
    out = negative ? -value : value;
    return true;
}

// Splits one CSV line into fields. Returns how many fields were found.
// Writes at most maxFields of them into out.
static int splitLine(const std::string& line, std::string out[], int maxFields) {
    std::stringstream ss(line);
    std::string field;
    int count = 0;
    while (std::getline(ss, field, ',')) {
        if (count < maxFields) {
            out[count] = field;
        }
        count++;
    }
    return count;
}

// Reads one line. Removes the \r that Windows line endings leave at the end,
// which would otherwise stick to the last field of the row.
static bool readLine(std::istream& in, std::string& line) {
    if (!std::getline(in, line)) {
        return false;
    }
    if (!line.empty() && line[line.size() - 1] == '\r') {
        line.erase(line.size() - 1);
    }
    return true;
}

// True if the line holds nothing but spaces and tabs, so it can be skipped
// like an empty line.
static bool isBlank(const std::string& line) {
    for (int i = 0; i < (int)line.size(); i++) {
        if (line[i] != ' ' && line[i] != '\t') {
            return false;
        }
    }
    return true;
}

// Reads the first line of a file and checks that it is the header row the
// loader expects. Without this check, a file missing its header would
// silently lose its first row of data.
static bool readHeader(std::istream& in, const std::string& file,
                       const std::string& expected) {
    std::string line;
    if (!readLine(in, line)) {
        std::cout << "Error: " << file << " is empty\n";
        return false;
    }
    // Excel's "CSV UTF-8" format starts the file with three marker bytes, a
    // byte order mark. They are not part of the header, so they are dropped.
    if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
        line.erase(0, 3);
    }
    if (line != expected) {
        std::cout << "Error: " << file << " line 1 should be the header row "
                  << expected << "\n";
        return false;
    }
    return true;
}

bool loadCampusData(const std::string& buildingsFile,
                    const std::string& pathwaysFile,
                    Graph& g) {
    // Everything is read into a new graph and copied into g only once both
    // files have loaded. A failed load leaves g as it was, and loading into a
    // graph that already holds data replaces that data.
    Graph loaded;

    std::ifstream bin(buildingsFile);
    if (!bin) {
        std::cout << "Error: could not open " << buildingsFile << "\n";
        return false;
    }

    std::string line;
    if (!readHeader(bin, buildingsFile, "id,code,name,lat,lon,x,y")) {
        return false;
    }

    int lineNumber = 1;
    while (readLine(bin, line)) {
        lineNumber++;
        if (isBlank(line)) {
            continue;
        }

        std::string f[7];
        int n = splitLine(line, f, 7);
        if (n != 7) {
            std::cout << "Error: " << buildingsFile << " line " << lineNumber
                      << " has " << n << " fields, expected 7\n";
            return false;
        }

        Building b;
        b.code = f[1];
        b.name = f[2];
        // f[3] and f[4] are lat and lon. The program works in meters, so they
        // are not read.
        if (!toInt(f[0], b.id) || !toDouble(f[5], b.x) || !toDouble(f[6], b.y)) {
            std::cout << "Error: " << buildingsFile << " line " << lineNumber
                      << " has a value that is not a number\n";
            return false;
        }

        if (b.id != loaded.buildings.size) {
            std::cout << "Error: " << buildingsFile << " line " << lineNumber
                      << " has id " << b.id << ", expected "
                      << loaded.buildings.size << "\n";
            return false;
        }

        loaded.buildings.push_back(b);
    }

    if (loaded.buildings.size == 0) {
        std::cout << "Error: " << buildingsFile << " has no buildings\n";
        return false;
    }

    // One empty list of pathways per building, so adj[i] exists before any
    // pathway is added to it.
    for (int i = 0; i < loaded.buildings.size; i++) {
        DynamicArray<Edge> empty;
        loaded.adj.push_back(empty);
    }

    std::ifstream pin(pathwaysFile);
    if (!pin) {
        std::cout << "Error: could not open " << pathwaysFile << "\n";
        return false;
    }

    if (!readHeader(pin, pathwaysFile,
                    "from,to,distance,has_stairs,is_closed,is_indoor,source")) {
        return false;
    }

    lineNumber = 1;
    int pathwayCount = 0;
    while (readLine(pin, line)) {
        lineNumber++;
        if (isBlank(line)) {
            continue;
        }

        std::string f[7];
        int n = splitLine(line, f, 7);
        if (n != 7) {
            std::cout << "Error: " << pathwaysFile << " line " << lineNumber
                      << " has " << n << " fields, expected 7\n";
            return false;
        }

        int from = 0;
        int to = 0;
        double distance = 0.0;
        if (!toInt(f[0], from) || !toInt(f[1], to) || !toDouble(f[2], distance)) {
            std::cout << "Error: " << pathwaysFile << " line " << lineNumber
                      << " has a value that is not a number\n";
            return false;
        }
        if ((f[3] != "0" && f[3] != "1") ||
            (f[4] != "0" && f[4] != "1") ||
            (f[5] != "0" && f[5] != "1")) {
            std::cout << "Error: " << pathwaysFile << " line " << lineNumber
                      << " has a flag that is not 0 or 1\n";
            return false;
        }

        if (from < 0 || from >= loaded.buildings.size ||
            to   < 0 || to   >= loaded.buildings.size) {
            std::cout << "Error: " << pathwaysFile << " line " << lineNumber
                      << " refers to building " << from << " or " << to
                      << ", which is outside 0 to " << loaded.buildings.size - 1
                      << "\n";
            return false;
        }
        if (from == to) {
            std::cout << "Error: " << pathwaysFile << " line " << lineNumber
                      << " joins building " << from << " to itself\n";
            return false;
        }

        if (distance <= 0.0) {
            std::cout << "Error: " << pathwaysFile << " line " << lineNumber
                      << " has distance " << distance << "\n";
            return false;
        }

        // A* relies on this rule (see data/README.md): no pathway can be
        // shorter than the straight line between its two buildings.
        double straightLine = heuristic(loaded, from, to);
        if (distance < straightLine) {
            std::cout << "Error: " << pathwaysFile << " line " << lineNumber
                      << " has distance " << distance << ", shorter than the "
                      << straightLine << " m straight line between buildings "
                      << from << " and " << to << "\n";
            return false;
        }

        Edge e;
        e.distance  = distance;
        e.hasStairs = (f[3] == "1");
        e.isClosed  = (f[4] == "1");
        e.isIndoor  = (f[5] == "1");
        // f[6] is the source of the distance. It is kept in the file as a
        // record of where each number came from, and is not used here.

        // One row of the file is one pathway, and a pathway can be walked in
        // either direction, so it is stored in both buildings' lists.
        e.to = to;
        loaded.adj[from].push_back(e);

        e.to = from;
        loaded.adj[to].push_back(e);
        pathwayCount++;
    }

    if (pathwayCount == 0) {
        std::cout << "Error: " << pathwaysFile << " has no pathways\n";
        return false;
    }

    g = loaded;
    return true;
}

// A pathway cannot be used if it is closed. If the user asked to avoid
// stairs, a pathway with steps cannot be used either. INDOOR_PREFERRED does
// not rule anything out; it changes the cost instead, in pathCost below.
bool isPathAccessible(const Edge& e, Preference pref) {
    if (e.isClosed) {
        return false;
    }
    if (pref == NO_STAIRS && e.hasStairs) {
        return false;
    }
    return true;
}

double pathCost(const Edge& e, Preference pref) {
    if (pref == INDOOR_PREFERRED && !e.isIndoor) {
        return e.distance * OUTDOOR_WEIGHT;
    }
    return e.distance;
}

double heuristic(const Graph& g, int from, int to) {
    double dx = g.buildings[from].x - g.buildings[to].x;
    double dy = g.buildings[from].y - g.buildings[to].y;
    return std::sqrt(dx * dx + dy * dy);
}
