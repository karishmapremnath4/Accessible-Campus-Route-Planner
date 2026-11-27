# Pseudocode for A* search

Written before the implementation, as the Dijkstra pseudocode was in
Iteration 2.

    INPUT:  Graph, starting building, destination building, preference
    OUTPUT: Shortest accessible route and total distance

    START
      IF starting building or destination is not a valid building number
          RETURN no route found

      Set cost of every building to infinity
      Set previous building of every building to none
      Mark every building as not finalized
      Set cost of starting building to 0

      Create a priority queue ordered by smallest estimated total
      Insert starting building with estimated total = heuristic(start, destination)

      WHILE priority queue is not empty
          Remove the building with the smallest estimated total

          IF building is already finalized
              Skip it
          Mark building as finalized
          Add 1 to the count of buildings examined

          IF building is the destination
              Stop searching

          FOR each neighboring building
              IF isPathAccessible(pathway, preference) is FALSE
                  Skip pathway

              new cost = cost of current building + pathCost(pathway, preference)
              IF new cost is smaller than stored cost of the neighbor
                  Update cost of the neighbor
                  Save previous building
                  estimated total = new cost + heuristic(neighbor, destination)
                  Add neighbor to priority queue with that estimated total

      IF destination is reachable
          Build the route by following the previous buildings backwards
          Add up the real distances along that route
          RETURN the route and the total distance
      ELSE
          RETURN no route found
    END

## How this differs from Dijkstra

Three lines, and nothing else:

1. The queue is ordered by the estimated total, not by the cost so far.
2. The starting building is inserted with `heuristic(start, destination)`
   rather than 0.
3. When a neighbor is improved it is queued with
   `new cost + heuristic(neighbor, destination)`.

What does not change: `cost` still holds the real accumulated cost. The
estimate only affects the order buildings come off the queue, never what is
stored. The distance reported to the user is still the sum of the real
pathway distances, which matters under INDOOR_PREFERRED where the cost being
minimized is not the distance.

## Why the answer is still the shortest route

`heuristic()` returns the straight-line distance between two buildings,
computed from their x and y values. No pathway in any of the three datasets is
shorter than the straight-line distance between the two buildings it joins, so
the estimate can never be larger than the distance actually remaining. An
estimate that never overestimates is what makes A* return the same shortest
distance as Dijkstra.

Underestimating is always safe. It costs speed, not correctness.
