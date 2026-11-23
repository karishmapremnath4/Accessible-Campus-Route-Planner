#include "menu.h"
#include <cctype>
#include <iomanip>
#include <iostream>

static std::string trim(const std::string& s) {
    int first = 0;
    int last = (int)s.size();
    while (first < last && std::isspace((unsigned char)s[first])) first++;
    while (last > first && std::isspace((unsigned char)s[last - 1])) last--;
    return s.substr(first, last - first);
}

static std::string toLower(const std::string& s) {
    std::string out = s;
    for (int i = 0; i < (int)out.size(); i++) {
        out[i] = (char)std::tolower((unsigned char)out[i]);
    }
    return out;
}

void showMenu() {
    std::cout << "\n=== Accessible Campus Route Planner ===\n"
              << "1. Display buildings\n"
              << "2. Find a route\n"
              << "3. Exit\n"
              << "Choose an option: ";
}

void displayBuildings(const Graph& g) {
    std::cout << "\n  #  Code  Name\n";
    for (int i = 0; i < g.buildings.size; i++) {
        const Building& b = g.buildings[i];
        std::cout << std::setw(3) << b.id << "  "
                  << std::left << std::setw(5) << b.code
                  << std::right << b.name << "\n";
    }
}

int findBuildingByName(const Graph& g, const std::string& name) {
    std::string wanted = toLower(trim(name));
    if (wanted.empty()) {
        return -1;
    }
    for (int i = 0; i < g.buildings.size; i++) {
        const Building& b = g.buildings[i];
        if (toLower(b.code) == wanted || toLower(b.name) == wanted) {
            return b.id;
        }
    }
    return -1;
}

bool getUserInput(const Graph& g, int& start, int& destination, Preference& pref) {
    std::string line;

    while (true) {
        std::cout << "Starting building (name or code): ";
        if (!std::getline(std::cin, line)) {
            return false;
        }
        start = findBuildingByName(g, line);
        if (start != -1) {
            break;
        }
        std::cout << "No building matches \"" << trim(line)
                  << "\". Choose \"Display buildings\" to see the names.\n";
    }

    while (true) {
        std::cout << "Destination building (name or code): ";
        if (!std::getline(std::cin, line)) {
            return false;
        }
        destination = findBuildingByName(g, line);
        if (destination != -1) {
            break;
        }
        std::cout << "No building matches \"" << trim(line)
                  << "\". Choose \"Display buildings\" to see the names.\n";
    }

    while (true) {
        std::cout << "Preference: 1 = shortest, 2 = no stairs, 3 = prefer tunnels: ";
        if (!std::getline(std::cin, line)) {
            return false;
        }
        std::string choice = trim(line);
        if (choice == "1") {
            pref = STANDARD;
            break;
        }
        if (choice == "2") {
            pref = NO_STAIRS;
            break;
        }
        if (choice == "3") {
            pref = INDOOR_PREFERRED;
            break;
        }
        std::cout << "Please enter 1, 2 or 3.\n";
    }

    return true;
}

void displayRoute(const Graph& g, const RouteResult& result) {
    if (!result.found) {
        std::cout << "No accessible route available.\n";
        return;
    }

    std::cout << "\nRoute:\n";
    for (int i = 0; i < result.buildings.size; i++) {
        const Building& b = g.buildings[result.buildings[i]];
        std::cout << (i + 1) << ". " << b.name << " (" << b.code << ")\n";
        if (i < result.pathways.size) {
            const Edge& step = result.pathways[i];
            std::cout << "     " << std::fixed << std::setprecision(0)
                      << step.distance << " m";
            if (step.hasStairs) {
                std::cout << ", has stairs";
            }
            if (step.isIndoor) {
                std::cout << ", through a tunnel";
            }
            std::cout << "\n";
        }
    }
    std::cout << "Total distance: " << std::fixed << std::setprecision(0)
              << result.distance << " m\n";
}