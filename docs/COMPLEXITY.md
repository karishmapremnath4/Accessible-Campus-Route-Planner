# Time and space complexity

V = number of buildings = 39
E = number of pathways = 151

Each pathway is stored twice, once in each direction, so the adjacency list
holds 302 entries. The graph is connected. Both searches stop as soon as the
destination comes off the queue.

## DynamicArray

| Operation | Best | Worst |
| --- | --- | --- |
| push_back | Theta(1) | Theta(n) |
| push_back over n calls | Theta(1) each | Theta(1) each |
| operator[] | Theta(1) | Theta(1) |
| copy | Theta(n) | Theta(n) |

push_back is Theta(n) when the array is full, because it has to copy
everything into a bigger block. The capacity doubles each time, so this gets
rarer as the array grows. Over n pushes the total work is Theta(n), which is
Theta(1) per push on average.

Space is Theta(n).

## MinHeap

n = number of items in the heap.

| Operation | Best | Worst |
| --- | --- | --- |
| push | Theta(1) | Theta(log n) |
| pop | Theta(1) | Theta(log n) |
| empty | Theta(1) | Theta(1) |

The heap is a tree of height log n. push moves an item up at most that far,
pop moves one down at most that far. Best case is when the item does not move.

Space is Theta(n).

## loadCampusData

Theta(V + E) time and Theta(V + E) space.

Each line is read once and split into 7 fields. There are V lines in
buildings.csv and E lines in pathways.csv.

## Dijkstra

| Case | Bound |
| --- | --- |
| Worst | Theta((V + E) log V) |
| Best | Theta(1) |
| Space | Theta(V + E) |

Worst case is when the destination is the last building taken off the queue.
Every building is pushed and popped and every pathway is checked. Each heap
operation costs log V.

Best case is when the start is the destination. It comes off the queue first
and the search stops.

Space is the adjacency list, plus one array each for distances, previous
buildings and finalised flags.

## A*

Same bounds as Dijkstra. The heuristic changes which buildings come off the
queue first, not how much each step costs. The heuristic itself is Theta(1):
one subtraction, two multiplications and a square root.

In practice A* looks at fewer buildings than Dijkstra, but that is a constant
factor on a graph this size, not a different bound. It has to be measured, by
counting how many buildings each one takes off the queue.

The heuristic never overestimates, because no pathway is shorter than the
straight-line distance between the two buildings it joins. This was checked for
all 151 pathways. That is what makes A* return the same shortest distance as
Dijkstra.
