#include "graph.h"
#include "menu.h"
#include "route.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    const char* pathways = (argc > 1) ? argv[1] : "data/pathways.csv";
    Graph g;
    if (!loadCampusData("data/buildings.csv", pathways, g)) {
        return 1;
    }

    while (true) {
        showMenu();
        std::string choice;
        if (!std::getline(std::cin, choice)) {
            break;
        }
        if (choice == "1") {
            displayBuildings(g);
        } else if (choice == "2") {
            int start = -1;
            int destination = -1;
            Preference pref = STANDARD;
            if (!getUserInput(g, start, destination, pref)) {
                break;
            }
            displayRoute(g, dijkstra(g, start, destination, pref));
        } else if (choice == "3") {
            break;
        } else {
            std::cout << "Please enter 1, 2 or 3.\n";
        }
    }
    return 0;
}