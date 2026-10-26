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
4. Calculate the shortest route that satisfies the preference, avoiding stairs when
   requested and skipping closed pathways.
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

The program reads its campus data from `data/` at startup.

## Sample input and output

Not yet available. A sample session will be added once the menu and the first search
algorithm are implemented.

## Repository structure

```
src/    C++ source files
data/   campus building and pathway data files
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

Iteration 1, project proposal. Design complete, implementation not yet started.

- [x] Problem definition, features, expected inputs and outputs
- [x] Data structures selected and justified
- [x] Major functions identified
- [x] Pseudocode for the main program flow and for Dijkstra's algorithm
- [x] Initial testing plan, 8 test cases
- [x] Repository created and branch structure set up
- [ ] Campus dataset prepared
- [ ] File loading and graph construction
- [ ] Dijkstra's algorithm
- [ ] A* search
- [ ] Menu interface and input validation
- [ ] Algorithm comparison

## Future improvements

Time-of-day building access, indoor floor-level routing, and a graphical map display are
noted as possible extensions beyond the scope of this project.

## AI tool use

Any assistance from AI tools is documented in `docs/AI_USE.md` according to the course
AI-use policy. Both members review, understand, explain, and test the code they submit.
