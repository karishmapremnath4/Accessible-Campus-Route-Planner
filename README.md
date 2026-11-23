# Accessible Campus Route Planner

A menu-driven C++ application that finds walking routes between campus buildings while
respecting a user's accessibility needs. The program models the campus as a graph, lets the
user choose to avoid stairs, and skips pathways that are temporarily closed, so the route it
returns is one the user can actually take rather than only the shortest distance.

Course project for EECE 7205, Fundamentals of Computer Engineering.

## Team

| Member | Area |
| --- | --- |
| Mithila Krishna Shashikala | User interface and Dijkstra's algorithm |
| Karishma Premnath | Campus data, graph representation, and A* search |

## Features

1. Load campus building and pathway data from a file at startup.
2. Display the list of all campus buildings.
3. Accept a starting building, a destination building, and an accessibility preference.
4. Calculate the shortest route that satisfies the preference. Three
   preferences are supported: the shortest route overall, a route that avoids
   stairs, and a route that favours the campus tunnels. Closed pathways are
   skipped under all three.
5. Display the recommended route, building by building, with the total distance.
6. Report clearly when no route satisfies the selected preference.
7. Compare Dijkstra's algorithm and A* search on the same route request.

## Tools and language

- C++17, standard library only. No external or third-party libraries.
- Compiler: g++ (GCC) or clang++.
- Editor: Visual Studio Code.
- Debugger: GDB on Linux and Windows, LLDB on macOS.
- Git and GitHub for version control and code review.

## Build

```
g++ -std=c++17 -Wall -Wextra -o planner src/*.cpp
```

## Run

```
./planner
```

Run the program from the top of the repository, because it reads its campus
data from `data/` using relative paths.

A different pathways file can be given as an argument, which is how the
no-route case is demonstrated:

```
./planner data/test_no_route_pathways.csv
```

## Tests

```
g++ -std=c++17 -Wall -Wextra -I src -o run_tests tests/tests.cpp src/graph.cpp src/dijkstra.cpp
./run_tests

g++ -std=c++17 -Wall -Wextra -I src -o run_route_tests tests/test_dijkstra.cpp src/graph.cpp src/dijkstra.cpp
./run_route_tests
```

Run both from the top of the repository. The first checks the loader,
`MinHeap`, `DynamicArray` and the test data files. The second checks the
routes Dijkstra returns. Each prints a line for every failed check and ends
with a summary.

## Sample input and output

The same two buildings, with and without stairs allowed.

```
Choose an option: 2
Starting building (name or code): SL
Destination building (name or code): EXP
Preference: 1 = shortest, 2 = no stairs, 3 = prefer tunnels: 1

Route:
Snell Library (SL)
   | 530 m, has stairs
   v
Interdisciplinary Science and Engineering Complex (ISEC)
   | 138 m
   v
EXP (EXP)
Total distance: 668 m
```

```
Choose an option: 2
Starting building (name or code): SL
Destination building (name or code): EXP
Preference: 1 = shortest, 2 = no stairs, 3 = prefer tunnels: 2

Route:
Snell Library (SL)
   | 86 m, through a tunnel
   v
Curry Student Center (CSC)
   | 348 m
   v
Badger and Rosen SquashBusters Center (SB)
   | 250 m
   v
EXP (EXP)
Total distance: 684 m
```

The shortest route crosses the ISEC pedestrian bridge, which has stairs. With
stairs excluded the program routes around at ground level instead, 16 m
longer.

## Repository structure

```
src/    C++ source files
data/   campus building and pathway data files
tests/  test program for the code in src/
docs/   project proposal and reports
```

## Branching

`main` holds the stable version. Each member works on a feature branch and opens a pull
request for the other member to review before merging.

- `feature/menu-interface`
- `feature/graph-data`
- `feature/dijkstra`
- `feature/astar`

## Current progress

Iteration 3. The program compiles, runs, and finds routes between any two
campus buildings under three accessibility preferences. A* search and the
comparison between the two algorithms are left for iteration 4.

- [x] Problem definition, features, expected inputs and outputs
- [x] Data structures selected and justified
- [x] Major functions identified
- [x] Pseudocode for the main program flow and for Dijkstra's algorithm
- [x] Initial testing plan, 8 test cases
- [x] Repository created and branch structure set up
- [x] Campus dataset prepared
- [x] File loading and graph construction
- [x] MinHeap, accessibility preferences and the A* heuristic
- [x] Tests for the loader, MinHeap, DynamicArray and the test data files
- [x] Dijkstra's algorithm
- [ ] A* search
- [x] Menu interface and input validation
- [ ] Algorithm comparison

The dataset holds 39 campus buildings and 151 pathways. It was prepared in
early November and committed to this repository on 20 November. Building
positions come from OpenStreetMap and 139 of the 151 distances were measured
by routing over the OpenStreetMap pedestrian network. See `data/README.md`
for the format and the source of every distance.

`loadCampusData()` reads both files into an adjacency list of 39 buildings and
302 directed entries, since each pathway is stored in both directions. Ten
building pairs have two routes between them, such as Snell Library to Curry
Student Center across the plaza or through the tunnel, and both are kept.

The data structures the routing algorithms use are implemented in this project
rather than taken from the Standard Library. `DynamicArray` replaces
`std::vector`, and `MinHeap`, a binary min-heap, replaces
`std::priority_queue`. The Standard Library is used only for console and file
input and output, text, and the square root function.

## Known limitations

- Eleven of the 151 distances are estimates rather than measurements: nine
  tunnel segments and the two routes over the ISEC pedestrian bridge, none of
  which OpenStreetMap maps. They are marked `estimated` in the `source` column.
- Twenty-two buildings are positioned at the centre of the building rather
  than at an entrance: six because OpenStreetMap records no entrance for them,
  and the sixteen added later (ids 23 to 38). Distances involving them read
  slightly long.
- Only one pathway is marked as having stairs. Deriving the flag automatically
  from OpenStreetMap was attempted and abandoned, as described in the project
  report.
- `DynamicArray::operator[]` does not check bounds. Index validation is done by
  the caller.

## Future improvements

Time-of-day building access, indoor floor-level routing, and a graphical map display are
noted as possible extensions beyond the scope of this project.

## AI tool use

Any assistance from AI tools is documented in `docs/AI_USE.md` according to the course
AI-use policy. Both members review, understand, explain, and test the code they submit.
