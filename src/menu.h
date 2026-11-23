#ifndef MENU_H
#define MENU_H

#include "graph.h"
#include "route.h"
#include <string>

void showMenu();
void displayBuildings(const Graph& g);
int findBuildingByName(const Graph& g, const std::string& name);
bool getUserInput(const Graph& g, int& start, int& destination, Preference& pref);
void displayRoute(const Graph& g, const RouteResult& result);

#endif